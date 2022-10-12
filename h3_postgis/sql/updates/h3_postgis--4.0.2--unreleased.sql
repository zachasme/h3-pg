/*
 * Copyright 2022 Bytes & Brains
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

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "ALTER EXTENSION h3 UPDATE TO 'unreleased'" to load this file. \quit

CREATE OR REPLACE FUNCTION
    h3_cells_to_multi_polygon_geometry(h3index[]) RETURNS geometry
AS $$ SELECT h3_cells_to_multi_polygon_wkb($1)::geometry $$ IMMUTABLE STRICT PARALLEL SAFE LANGUAGE SQL;

CREATE OR REPLACE FUNCTION
    h3_cells_to_multi_polygon_geography(h3index[]) RETURNS geography
AS $$ SELECT h3_cells_to_multi_polygon_wkb($1)::geography $$ IMMUTABLE STRICT PARALLEL SAFE LANGUAGE SQL;

CREATE OR REPLACE AGGREGATE h3_cells_to_multi_polygon_geometry(h3index) (
    sfunc = array_append,
    stype = h3index[],
    finalfunc = h3_cells_to_multi_polygon_geometry,
    parallel = safe
);

CREATE OR REPLACE AGGREGATE h3_cells_to_multi_polygon_geography(h3index) (
    sfunc = array_append,
    stype = h3index[],
    finalfunc = h3_cells_to_multi_polygon_geography,
    parallel = safe
);
