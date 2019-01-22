/*
 * Copyright 2018 Bytes & Brains
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <postgres.h>       // Datum, etc.
#include <fmgr.h>           // PG_FUNCTION_ARGS, etc.
#include <funcapi.h>        // Definitions for functions which return sets
#include <utils/array.h>    // Arrays
#include <utils/memutils.h> // MaxAllocSize

#include <h3/h3api.h> // Main H3 include
#include "extension.h"

// Returns the parent (coarser) index containing given index
PG_FUNCTION_INFO_V1(h3_to_parent);
Datum h3_to_parent(PG_FUNCTION_ARGS)
{
    H3Index *parent;

    // get function arguments
    H3Index *origin = PG_GETARG_H3_INDEX_P(0);
    int resolution = PG_GETARG_INT32(1);
    if (resolution == -1)
    { // resolution parameter not set
        resolution = h3GetResolution(*origin) - 1;
    }

    // get parent
    parent = palloc(sizeof(H3Index));
    *parent = h3ToParent(*origin, resolution);
    if (!*parent)
    {
        ereport(
            ERROR,
            (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
             errmsg("Invalid resolution %d.", resolution),
             errdetail(
                 "Current backend only has 16 resolutions numbered 0 through 15."),
             errhint("Choose valid resolution.")));
    }

    PG_RETURN_H3_INDEX_P(parent);
}

// Returns children indexes at given resolution (or next resolution if none given)
PG_FUNCTION_INFO_V1(h3_to_children);
Datum h3_to_children(PG_FUNCTION_ARGS)
{
    // stuff done only on the first call of the function
    if (SRF_IS_FIRSTCALL())
    {
        int maxSize;
        int size;
        H3Index *children;

        // create a function context for cross-call persistence
        FuncCallContext *funcctx = SRF_FIRSTCALL_INIT();

        // switch to memory context appropriate for multiple function calls
        MemoryContext oldcontext =
            MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        // BEGIN One-time setup code

        // ensure valid resolution target
        H3Index *origin = PG_GETARG_H3_INDEX_P(0);
        int resolution = PG_GETARG_INT32(1);
        if (resolution == -1)
        { // resolution parameter not set
            resolution = h3GetResolution(*origin) + 1;
        }

        if (resolution > 15)
        {
            ereport(
                ERROR,
                (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
                 errmsg("Maximum resolution exceeded."),
                 errdetail(
                     "Current backend only has 16 resolutions numbered 0 through 15."),
                 errhint("Reduce resolution.")));
        }

        maxSize = maxH3ToChildrenSize(*origin, resolution);
        size = maxSize * sizeof(H3Index);

        if (size > MaxAllocSize)
        {
            ereport(
                ERROR,
                (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
                 errmsg("Cannot allocate requested memory. Try using h3_to_children_slow().")));
        }

        children = palloc(size);
        h3ToChildren(*origin, resolution, children);

        funcctx->user_fctx = children;
        funcctx->max_calls = maxSize;

        // END One-time setup code

        MemoryContextSwitchTo(oldcontext);
    }

    SRF_RETURN_H3_INDEXES_FROM_USER_FCTX();
}

PG_FUNCTION_INFO_V1(h3_compact);
Datum h3_compact(PG_FUNCTION_ARGS)
{
    if (SRF_IS_FIRSTCALL())
    {
        FuncCallContext *funcctx = SRF_FIRSTCALL_INIT();
        MemoryContext oldcontext =
            MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        ArrayType *array = PG_GETARG_ARRAYTYPE_P(0);

        int arrayLength = ArrayGetNItems(ARR_NDIM(array), ARR_DIMS(array));
        H3Index *h3set = palloc(sizeof(H3Index) * arrayLength);
        H3Index *idx = (H3Index *)ARR_DATA_PTR(array);

        int maxSize = arrayLength;
        H3Index *compactedSet = palloc0(maxSize * sizeof(H3Index));

        // Extract data from array into h3set, and wipe compactedSet memory
        for (int i = 0; i < arrayLength; i++)
        {
            h3set[i] = fetch_att(idx, true, sizeof(H3Index));
            idx++;
        }

        if (compact(h3set, compactedSet, arrayLength) != 0)
        {
            elog(ERROR, "Something went wrong during compacting");
        }

        funcctx->user_fctx = compactedSet;
        funcctx->max_calls = maxSize;
        MemoryContextSwitchTo(oldcontext);
    }

    SRF_RETURN_H3_INDEXES_FROM_USER_FCTX();
}

PG_FUNCTION_INFO_V1(h3_uncompact);
Datum h3_uncompact(PG_FUNCTION_ARGS)
{
    if (SRF_IS_FIRSTCALL())
    {
        int maxSize;
        H3Index *uncompactedSet;

        FuncCallContext *funcctx = SRF_FIRSTCALL_INIT();
        MemoryContext oldcontext =
            MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        ArrayType *array = PG_GETARG_ARRAYTYPE_P(0);
        int resolution = PG_GETARG_INT32(1);

        int arrayLength = ArrayGetNItems(ARR_NDIM(array), ARR_DIMS(array));
        H3Index *h3set = palloc(sizeof(H3Index) * arrayLength);
        H3Index *idx = (H3Index *)ARR_DATA_PTR(array);

        // Extract data from array into h3set, and wipe compactedSet memory
        for (int i = 0; i < arrayLength; i++)
        {
            h3set[i] = fetch_att(idx, true, sizeof(H3Index));
            idx++;
        }

        if (resolution == -1)
        { // resolution parameter not set
            int highRes = 0;
            // Find highest resolution in the given set
            for (int i = 0; i < arrayLength; i++)
            {
                int curRes = h3GetResolution(h3set[i]);
                if (curRes > highRes)
                {
                    highRes = curRes;
                }
            }
            // If the highest resolution is the maximun allowed, uncompact to that
            // Else uncompact one step further than the highest resolution
            resolution = (highRes == 15 ? highRes : highRes + 1);
        }

        maxSize = maxUncompactSize(h3set, arrayLength, resolution);
        uncompactedSet = palloc0(maxSize * sizeof(H3Index));

        if (uncompact(h3set, arrayLength, uncompactedSet, maxSize, resolution) !=
            0)
        {
            ereport(ERROR,
                    (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
                     errmsg("Something went wrong during uncompacting"),
                     errdetail("This may be caused by choosing a lower resolution "
                               "than some of the indexes"),
                     errhint("Check that it is called with the proper resolution")));
        }

        funcctx->user_fctx = uncompactedSet;
        funcctx->max_calls = maxSize;
        MemoryContextSwitchTo(oldcontext);
    }

    SRF_RETURN_H3_INDEXES_FROM_USER_FCTX();
}