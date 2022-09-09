/*
 * Copyright 2018-2022 Bytes & Brains
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
\echo Use "ALTER EXTENSION h3_postgis UPDATE TO 'unreleased'" to load this file. \quit

CREATE OR REPLACE FUNCTION
    h3_cell_to_boundary_wkb(cell h3index, split_at_meridian boolean DEFAULT FALSE) RETURNS bytea
AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE; COMMENT ON FUNCTION
    h3_cell_to_boundary_wkb(h3index, boolean)
IS 'Finds the boundary of the index, returns EWKB, second argument splits polygon when crossing 180th meridian';

CREATE OR REPLACE FUNCTION h3_cell_to_boundary_geometry(h3index, split boolean DEFAULT FALSE) RETURNS geometry
  AS $$ SELECT ST_SetSRID(h3_cell_to_boundary_wkb($1, $2)::geometry, 4326) $$ IMMUTABLE STRICT PARALLEL SAFE LANGUAGE SQL;

CREATE OR REPLACE FUNCTION h3_cell_to_boundary_geography(h3index, split boolean DEFAULT FALSE) RETURNS geography
  AS $$ SELECT h3_cell_to_boundary_geometry_wkb($1, $2)::geography $$ IMMUTABLE STRICT PARALLEL SAFE LANGUAGE SQL;
