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

#include <postgres.h>            // Datum, etc.
#include <fmgr.h>                // PG_FUNCTION_ARGS, etc.
#include <funcapi.h>             // Definitions for functions which return sets
#include <access/htup_details.h> // Needed to return HeapTuple
#include <utils/array.h>         // Arrays
#include <utils/geo_decls.h>     // making native points

#include <h3/h3api.h> // Main H3 include
#include "h3-pg.h"

/**
 * k-rings produces indices within k distance of the origin index.
 *
 * k-ring 0 is defined as the origin index, k-ring 1 is defined as k-ring 0 and
 * all neighboring indices, and so on.
 *
 * Output is placed in the provided array in no particular order.
 * There may be fewer elements in output, as can happen when crossing a
 * pentagon.
 */
PG_FUNCTION_INFO_V1(h3_k_ring);
Datum h3_k_ring(PG_FUNCTION_ARGS)
{
    if (SRF_IS_FIRSTCALL())
    {
        FuncCallContext *funcctx = SRF_FIRSTCALL_INIT();
        MemoryContext oldcontext =
            MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        // get function arguments
        H3Index *origin = PG_GETARG_H3_INDEX_P(0);
        int k = PG_GETARG_INT32(1);

        // produce indices into allocated memory
        int maxSize = maxKringSize(k);
        H3Index *indices = palloc(maxSize * sizeof(H3Index));
        kRing(*origin, k, indices);

        funcctx->user_fctx = indices;
        funcctx->max_calls = maxSize;
        MemoryContextSwitchTo(oldcontext);
    }

    SRF_RETURN_H3_INDEXES_FROM_USER_FCTX();
}

/**
 * k-rings produces indices within k distance of the origin index.
 *
 * k-ring 0 is defined as the origin index, k-ring 1 is defined as k-ring 0 and
 * all neighboring indices, and so on.
 *
 * Output is placed in the provided array in no particular order.
 * There may be fewer elements in output, as can happen when crossing a
 * pentagon.
 */
PG_FUNCTION_INFO_V1(h3_k_ring_distances);
Datum h3_k_ring_distances(PG_FUNCTION_ARGS)
{
    if (SRF_IS_FIRSTCALL())
    {
        FuncCallContext *funcctx = SRF_FIRSTCALL_INIT();
        MemoryContext oldcontext =
            MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);
        TupleDesc tuple_desc;

        // get function arguments
        H3Index *origin = PG_GETARG_H3_INDEX_P(0);
        int k = PG_GETARG_INT32(1);

        // Allocate memory for the indices, the distances and the tuple used for
        // returning
        int maxSize = maxKringSize(k);
        hexDistanceTuple *user_fctx = palloc(sizeof(hexDistanceTuple));
        user_fctx->indices = palloc(maxSize * sizeof(H3Index));
        user_fctx->distances = palloc(maxSize * sizeof(int));

        kRingDistances(*origin, k, user_fctx->indices, user_fctx->distances);

        ENSURE_TYPEFUNC_COMPOSITE(get_call_result_type(fcinfo, NULL, &tuple_desc));

        funcctx->tuple_desc = BlessTupleDesc(tuple_desc);
        funcctx->max_calls = maxSize;
        funcctx->user_fctx = user_fctx;

        MemoryContextSwitchTo(oldcontext);
    }

    SRF_RETURN_H3_INDEX_DISTANCES_FROM_USER_FCTX();
}

/**
 * hexRange produces indexes within k distance of the origin index. Output
 * behavior is undefined when one of the indexes returned by this function is a
 * pentagon or is in the pentagon distortion area.
 *
 * k-ring 0 is defined as the origin index, k-ring 1 is defined as k-ring 0 and
 * all neighboring indexes, and so on.
 *
 * Output is placed in the provided array in order of increasing distance from
 * the origin.
 *
 * Throws if pentagonal distortion is encountered.
 */
PG_FUNCTION_INFO_V1(h3_hex_range);
Datum h3_hex_range(PG_FUNCTION_ARGS)
{
    if (SRF_IS_FIRSTCALL())
    {
        FuncCallContext *funcctx = SRF_FIRSTCALL_INIT();
        MemoryContext oldcontext =
            MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        // get function arguments
        H3Index *origin = PG_GETARG_H3_INDEX_P(0);
        int k = PG_GETARG_INT32(1);

        // produce indices into allocated memory
        int maxSize = maxKringSize(k);
        H3Index *indices = palloc(maxSize * sizeof(H3Index));

        if (hexRange(*origin, k, indices) != 0)
        {
            ereport(
                ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("Pentagon encountered"),
                 errdetail("This method is undefined when it encounters pentagons"),
                 errhint("Try using k_ring")));
        }

        funcctx->user_fctx = indices;
        funcctx->max_calls = maxSize;
        MemoryContextSwitchTo(oldcontext);
    }

    SRF_RETURN_H3_INDEXES_FROM_USER_FCTX();
}

/**
 * hexRangeDistances produces indexes within k distance of the origin index.
 * Output behavior is undefined when one of the indexes returned by this
 * function is a pentagon or is in the pentagon distortion area.
 *
 * k-ring 0 is defined as the origin index, k-ring 1 is defined as k-ring 0 and
 * all neighboring indexes, and so on.
 *
 * Output is placed in the provided array in order of increasing distance from
 * the origin. The distances in hexagons is placed in the distances array at the
 * same offset.
 *
 * Throws if pentagonal distortion is encountered.
 *
 */
PG_FUNCTION_INFO_V1(h3_hex_range_distances);
Datum h3_hex_range_distances(PG_FUNCTION_ARGS)
{
    if (SRF_IS_FIRSTCALL())
    {
        FuncCallContext *funcctx = SRF_FIRSTCALL_INIT();
        MemoryContext oldcontext =
            MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        TupleDesc tuple_desc;

        // get function arguments
        H3Index *origin = PG_GETARG_H3_INDEX_P(0);
        int k = PG_GETARG_INT32(1);

        // Allocate memory for the indices, the distances and the tuple used for
        // returning
        int maxSize = maxKringSize(k);
        H3Index *indices = palloc(maxSize * sizeof(H3Index));
        int *distances = palloc(maxSize * sizeof(int));
        hexDistanceTuple *user_fctx = palloc(sizeof(hexDistanceTuple));

        if (hexRangeDistances(*origin, k, indices, distances) != 0)
        {
            ereport(
                ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("Pentagon encountered"),
                 errdetail("This method is undefined when it encounters pentagons"),
                 errhint("Try using k_ring_distances")));
        }

        ENSURE_TYPEFUNC_COMPOSITE(get_call_result_type(fcinfo, NULL, &tuple_desc));

        user_fctx->indices = indices;
        user_fctx->distances = distances;
        funcctx->tuple_desc = BlessTupleDesc(tuple_desc);
        funcctx->max_calls = maxSize;
        funcctx->user_fctx = user_fctx;

        MemoryContextSwitchTo(oldcontext);
    }

    SRF_RETURN_H3_INDEX_DISTANCES_FROM_USER_FCTX();
}

/**
 * hexRanges takes an array of input hex IDs and a max k-ring and returns an
 * array of hexagon IDs sorted first by the original hex IDs and then by the
 * k-ring (0 to max), with no guaranteed sorting within each k-ring group.
 *
 * Throws if pentagonal distortion was encountered.
 */
PG_FUNCTION_INFO_V1(h3_hex_ranges);
Datum h3_hex_ranges(PG_FUNCTION_ARGS)
{
    if (SRF_IS_FIRSTCALL())
    {
        FuncCallContext *funcctx = SRF_FIRSTCALL_INIT();
        MemoryContext oldcontext =
            MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        // get function arguments
        ArrayType *array = PG_GETARG_ARRAYTYPE_P(0);
        int k = PG_GETARG_INT32(1);

        int length = ArrayGetNItems(ARR_NDIM(array), ARR_DIMS(array));
        H3Index *h3Set = palloc(sizeof(H3Index) * length);
        H3Index *idx = (H3Index *)ARR_DATA_PTR(array);

        int maxSize = maxKringSize(k) * length;
        H3Index *indices = palloc(maxSize * sizeof(H3Index));

        for (int i = 0; i < length; i++)
        {
            h3Set[i] = fetch_att(idx, true, sizeof(H3Index));
            idx++;
        }

        if (hexRanges(h3Set, length, k, indices) != 0)
        {
            ereport(
                ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("Pentagon encountered"),
                 errdetail("This method is undefined when it encounters pentagons"),
                 errhint("Try using k_ring")));
        }

        funcctx->user_fctx = indices;
        funcctx->max_calls = maxSize;
        MemoryContextSwitchTo(oldcontext);
    }

    SRF_RETURN_H3_INDEXES_FROM_USER_FCTX();
}

/**
 * Produces the hollow hexagonal ring centered at origin with sides of length k.
 *
 * Throws if pentagonal distortion was encountered.
 */
PG_FUNCTION_INFO_V1(h3_hex_ring);
Datum h3_hex_ring(PG_FUNCTION_ARGS)
{
    if (SRF_IS_FIRSTCALL())
    {
        FuncCallContext *funcctx = SRF_FIRSTCALL_INIT();
        MemoryContext oldcontext =
            MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        // get function arguments
        H3Index *indices;
        H3Index *origin = PG_GETARG_H3_INDEX_P(0);
        int k = PG_GETARG_INT32(1);

        // Find the size of the ring. If k is 0, then it is the same as k_ring.
        // If k is larger than 0, the ring is the size of the circle with k, minus
        // the circle with k-1
        int maxSize = maxKringSize(k);
        if (k > 0)
        {
            maxSize -= maxKringSize(k - 1);
        }
        indices = palloc(maxSize * sizeof(H3Index));

        if (hexRing(*origin, k, indices) != 0)
        {
            ereport(
                ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("Pentagon encountered"),
                 errdetail("This method is undefined when it encounters pentagons")));
        }

        funcctx->user_fctx = indices;
        funcctx->max_calls = maxSize;
        MemoryContextSwitchTo(oldcontext);
    }

    SRF_RETURN_H3_INDEXES_FROM_USER_FCTX();
}

/**
 * Returns the distance in grid cells between the two indexes.
 *
 * Returns a negative number if finding the distance failed.
 * Finding the distance can fail because the two indexes are not comparable
 * (different resolutions), too far apart, or are separated by pentagonal
 * distortion. This is the same set of limitations as the local IJ coordinate
 * space functions.
 */
PG_FUNCTION_INFO_V1(h3_distance);
Datum h3_distance(PG_FUNCTION_ARGS)
{
    H3Index *originIndex = PG_GETARG_H3_INDEX_P(0);
    H3Index *h3Index = PG_GETARG_H3_INDEX_P(1);
    int distance;

    distance = h3Distance(*originIndex, *h3Index);

    PG_RETURN_INT32(distance);
}

/**
 * Given two H3 indexes, return the line of indexes between them (inclusive).
 *
 * This function may fail to find the line between two indexes, for
 * example if they are very far apart. It may also fail when finding
 * distances for indexes on opposite sides of a pentagon.
 */
PG_FUNCTION_INFO_V1(h3_line);
Datum h3_line(PG_FUNCTION_ARGS)
{
    if (SRF_IS_FIRSTCALL())
    {
        FuncCallContext *funcctx = SRF_FIRSTCALL_INIT();
        MemoryContext oldcontext =
            MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        // get function arguments
        H3Index *start = PG_GETARG_H3_INDEX_P(0);
        H3Index *end = PG_GETARG_H3_INDEX_P(1);
        int size = h3LineSize(*start, *end);
        H3Index *indices = palloc(size * sizeof(H3Index));

        if (h3Line(*start, *end, indices) != 0)
        {
            ereport(
                ERROR,
                (errcode(ERRCODE_EXTERNAL_ROUTINE_EXCEPTION),
                 errmsg("Failed to get line")));
        }

        funcctx->user_fctx = indices;
        funcctx->max_calls = size;
        MemoryContextSwitchTo(oldcontext);
    }

    SRF_RETURN_H3_INDEXES_FROM_USER_FCTX();
}

/**
 * Produces local IJ coordinates for an H3 index anchored by an origin.
 *
 * This function is experimental, and its output is not guaranteed to be
 * compatible across different versions of H3.
 */
PG_FUNCTION_INFO_V1(h3_experimental_h3_to_local_ij);
Datum h3_experimental_h3_to_local_ij(PG_FUNCTION_ARGS)
{
    H3Index *origin = PG_GETARG_H3_INDEX_P(0);
    H3Index *index = PG_GETARG_H3_INDEX_P(1);

    Point *point = (Point *)palloc(sizeof(Point));
    CoordIJ coord;
    experimentalH3ToLocalIj(*origin, *index, &coord);

    point->x = coord.i;
    point->y = coord.j;

    PG_RETURN_POINT_P(point);
}

/**
 * Produces an H3 index from local IJ coordinates anchored by an origin.
 *
 * This function is experimental, and its output is not guaranteed to be
 * compatible across different versions of H3.
 */
PG_FUNCTION_INFO_V1(h3_experimental_local_ij_to_h3);
Datum h3_experimental_local_ij_to_h3(PG_FUNCTION_ARGS)
{
    H3Index *origin = PG_GETARG_H3_INDEX_P(0);
    Point *point = PG_GETARG_POINT_P(1);

    H3Index *index = (H3Index *)palloc(sizeof(H3Index));

    CoordIJ coord;
    coord.i = point->x;
    coord.j = point->y;

    experimentalLocalIjToH3(*origin, &coord, index);

    PG_RETURN_H3_INDEX_P(index);
}
