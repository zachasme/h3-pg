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

#include <postgres.h> // Datum, etc.
#include <fmgr.h>     // PG_FUNCTION_ARGS, etc.
#include <funcapi.h>  // Definitions for functions which return sets

#include <h3/h3api.h> // Main H3 include
#include "h3-pg.h"

// Converts degrees to radians
PG_FUNCTION_INFO_V1(h3_degs_to_rads);
Datum h3_degs_to_rads(PG_FUNCTION_ARGS)
{
    double degrees = PG_GETARG_FLOAT8(0);
    double radians = degsToRads(degrees);
    PG_RETURN_FLOAT8(radians);
}

// Converts radians to degrees
PG_FUNCTION_INFO_V1(h3_rads_to_degs);
Datum h3_rads_to_degs(PG_FUNCTION_ARGS)
{
    double radians = PG_GETARG_FLOAT8(0);
    double degrees = radsToDegs(radians);
    PG_RETURN_FLOAT8(degrees);
}

// Average hexagon area in square kilometers at the given resolution
PG_FUNCTION_INFO_V1(h3_hex_area_km2);
Datum h3_hex_area_km2(PG_FUNCTION_ARGS)
{
    int resolution = PG_GETARG_INT32(0);
    double area = hexAreaKm2(resolution);
    PG_RETURN_FLOAT8(area);
}

// Average hexagon area in square meters at the given resolution
PG_FUNCTION_INFO_V1(h3_hex_area_m2);
Datum h3_hex_area_m2(PG_FUNCTION_ARGS)
{
    int resolution = PG_GETARG_INT32(0);
    double area = hexAreaM2(resolution);
    PG_RETURN_FLOAT8(area);
}

// Average hexagon edge length in kilometers at the given resolution
PG_FUNCTION_INFO_V1(h3_edge_length_km);
Datum h3_edge_length_km(PG_FUNCTION_ARGS)
{
    int resolution = PG_GETARG_INT32(0);
    double length = edgeLengthKm(resolution);
    PG_RETURN_FLOAT8(length);
}

// Average hexagon edge length in meters at the given resolution
PG_FUNCTION_INFO_V1(h3_edge_length_m);
Datum h3_edge_length_m(PG_FUNCTION_ARGS)
{
    int resolution = PG_GETARG_INT32(0);
    double length = edgeLengthM(resolution);
    PG_RETURN_FLOAT8(length);
}

// Number of unique H3 indexes at the given resolution
PG_FUNCTION_INFO_V1(h3_num_hexagons);
Datum h3_num_hexagons(PG_FUNCTION_ARGS)
{
    int resolution = PG_GETARG_INT32(0);
    unsigned long long retVal = numHexagons(resolution);
    PG_RETURN_INT64(retVal);
}

// Provides all resolution 0 indexes
PG_FUNCTION_INFO_V1(h3_get_res_0_indexes);
Datum h3_get_res_0_indexes(PG_FUNCTION_ARGS)
{
    if (SRF_IS_FIRSTCALL())
    {
        FuncCallContext *funcctx = SRF_FIRSTCALL_INIT();
        MemoryContext oldcontext =
            MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        int count = res0IndexCount();
        H3Index *indexes = palloc(sizeof(H3Index) * count);
        getRes0Indexes(indexes);

        funcctx->user_fctx = indexes;
        funcctx->max_calls = count;
        MemoryContextSwitchTo(oldcontext);
    }

    SRF_RETURN_H3_INDEXES_FROM_USER_FCTX();
}
