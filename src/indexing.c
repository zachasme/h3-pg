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

#include <postgres.h>        // Datum, etc.
#include <fmgr.h>            // PG_FUNCTION_ARGS, etc.
#include <utils/geo_decls.h> // making native points

#include <h3/h3api.h> // Main H3 include
#include "h3-pg.h"

// Indexes the location at the specified resolution
PG_FUNCTION_INFO_V1(h3_geo_to_h3);
Datum h3_geo_to_h3(PG_FUNCTION_ARGS)
{
    Point *geo = PG_GETARG_POINT_P(0);
    int resolution = PG_GETARG_INT32(1);

    H3Index *idx = palloc(sizeof(H3Index));
    GeoCoord location;
    location.lon = degsToRads(geo->x);
    location.lat = degsToRads(geo->y);

    *idx = geoToH3(&location, resolution);

    if (*idx == 0)
    {
        ereport(
            ERROR,
            (errcode(ERRCODE_EXTERNAL_ROUTINE_EXCEPTION),
             errmsg("Indexing failed"),
             errdetail("GeoCoord could not be indexed at specified resolution."),
             errhint("Reduce resolution.")));
    }

    PG_RETURN_H3_INDEX_P(idx);
}

// Finds the centroid of the index
PG_FUNCTION_INFO_V1(h3_h3_to_geo);
Datum h3_h3_to_geo(PG_FUNCTION_ARGS)
{
    H3Index *idx = PG_GETARG_H3_INDEX_P(0);

    Point *geo = palloc(sizeof(Point));
    GeoCoord center;
    h3ToGeo(*idx, &center);

    geo->x = radsToDegs(center.lon);
    geo->y = radsToDegs(center.lat);

    PG_RETURN_POINT_P(geo);
}

// Finds the boundary of the index
PG_FUNCTION_INFO_V1(h3_h3_to_geo_boundary);
Datum h3_h3_to_geo_boundary(PG_FUNCTION_ARGS)
{
    H3Index *idx = PG_GETARG_H3_INDEX_P(0);

    int size;
    POLYGON *polygon;
    GeoBoundary boundary;
    h3ToGeoBoundary(*idx, &boundary);

    size = offsetof(POLYGON, p) + sizeof(polygon->p[0]) * boundary.numVerts;
    polygon = (POLYGON *)palloc(size);
    SET_VARSIZE(polygon, size);
    polygon->npts = boundary.numVerts;

    for (int v = 0; v < boundary.numVerts; v++)
    {
        polygon->p[v].x = radsToDegs(boundary.verts[v].lon);
        polygon->p[v].y = radsToDegs(boundary.verts[v].lat);
    }

    PG_RETURN_POLYGON_P(polygon);
}