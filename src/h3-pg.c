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

#include <math.h> // cos, sin, etc.
#include <assert.h>

#include <postgres.h>            // Datum, etc.
#include <fmgr.h>                // PG_FUNCTION_ARGS, etc.
#include <funcapi.h>             // Definitions for functions which return sets
#include <access/htup_details.h> // Needed to return HeapTuple

#include <h3/h3api.h> // Main H3 include
#include "h3-pg.h"

// should only be in ONE file
PG_MODULE_MAGIC;

static_assert(
    H3_VERSION_MAJOR == 3 && H3_VERSION_MINOR >= 3,
    "Installed H3 must be at least version 3.3.0"
);

/**
 * Set-Returning-Function assume user fctx contains indices
 * will skip missing (all zeros) indices
 */
Datum srf_return_h3_indexes_from_user_fctx(PG_FUNCTION_ARGS)
{
    FuncCallContext *funcctx = SRF_PERCALL_SETUP();
    int call_cntr = funcctx->call_cntr;
    int max_calls = funcctx->max_calls;

    H3Index *indices = (H3Index *)funcctx->user_fctx;

    // skip missing indices (all zeros)
    while (call_cntr < max_calls && !indices[call_cntr])
    {
        funcctx->call_cntr = ++call_cntr;
    };

    if (call_cntr < max_calls)
    {
        Datum result = PointerGetDatum(&indices[call_cntr]);
        SRF_RETURN_NEXT(funcctx, result);
    }
    else
    {
        SRF_RETURN_DONE(funcctx);
    }
}

/**
 * Returns hex/distance tuples from user_fctx
 * will skip missing (all zeros) indices
 */
Datum srf_return_h3_index_distances_from_user_fctx(PG_FUNCTION_ARGS)
{
    FuncCallContext *funcctx = SRF_PERCALL_SETUP();
    int call_cntr = funcctx->call_cntr;
    int max_calls = funcctx->max_calls;

    hexDistanceTuple *user_fctx = funcctx->user_fctx;
    H3Index *indices = user_fctx->indices;
    int *distances = user_fctx->distances;

    // skip missing indices (all zeros)
    while (!indices[call_cntr])
    {
        funcctx->call_cntr = ++call_cntr;
    };

    if (call_cntr < max_calls)
    {
        TupleDesc tuple_desc = funcctx->tuple_desc;
        Datum values[2];
        bool nulls[2] = {false};
        HeapTuple tuple;
        Datum result;

        values[0] = PointerGetDatum(&indices[call_cntr]);
        values[1] = Int32GetDatum(distances[call_cntr]);

        tuple = heap_form_tuple(tuple_desc, values, nulls);
        result = HeapTupleGetDatum(tuple);

        SRF_RETURN_NEXT(funcctx, result);
    }
    else
    {
        SRF_RETURN_DONE(funcctx);
    }
}

/**
 * @brief haversineDistance finds the
 * [great-circle distance](https://en.wikipedia.org/wiki/Great-circle_distance)
 * between two points on a sphere.
 * @see https://en.wikipedia.org/wiki/Haversine_formula.
 *
 * Copied from H3
 */

// mean Earth radius
#define R 6371.0088

PG_FUNCTION_INFO_V1(h3_haversine_distance);
Datum h3_haversine_distance(PG_FUNCTION_ARGS)
{
    H3Index *h3HQ1 = PG_GETARG_H3_INDEX_P(0);
    H3Index *h3HQ2 = PG_GETARG_H3_INDEX_P(1);

    double distance;
    double dx, dy, dz, th1, ph1, th2, ph2;

    GeoCoord geoHQ1, geoHQ2;
    h3ToGeo(*h3HQ1, &geoHQ1);
    h3ToGeo(*h3HQ2, &geoHQ2);

    th1 = geoHQ1.lat;
    ph1 = geoHQ1.lon;
    th2 = geoHQ2.lat;
    ph2 = geoHQ2.lon;

    ph1 -= ph2;

    dz = sin(th1) - sin(th2);
    dx = cos(ph1) * cos(th1) - cos(th2);
    dy = sin(ph1) * cos(th1);
    distance = asin(sqrt(dx * dx + dy * dy + dz * dz) / 2) * 2 * R;

    PG_RETURN_FLOAT8(distance);
}
