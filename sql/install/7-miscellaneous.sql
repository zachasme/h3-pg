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

-- Miscellaneous H3 functions (miscellaneous.c)

CREATE OR REPLACE FUNCTION h3_hex_area_km2(resolution integer) RETURNS float
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
    COMMENT ON FUNCTION h3_hex_area_km2(resolution integer) IS
'Average hexagon area in square kilometers at the given resolution.';

CREATE OR REPLACE FUNCTION h3_hex_area_m2(resolution integer) RETURNS float
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
    COMMENT ON FUNCTION h3_hex_area_m2(resolution integer) IS
'Average hexagon area in square meters at the given resolution.';

CREATE OR REPLACE FUNCTION h3_edge_length_km(resolution integer) RETURNS float
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
    COMMENT ON FUNCTION h3_edge_length_km(resolution integer) IS
'Average hexagon edge length in kilometers at the given resolution.';

CREATE OR REPLACE FUNCTION h3_edge_length_m(resolution integer) RETURNS float
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
    COMMENT ON FUNCTION h3_edge_length_m(resolution integer) IS
'Average hexagon edge length in meters at the given resolution.';

CREATE OR REPLACE FUNCTION h3_num_hexagons(resolution integer) RETURNS bigint
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
    COMMENT ON FUNCTION h3_num_hexagons(resolution integer) IS
'Number of unique H3 indexes at the given resolution.';

CREATE OR REPLACE FUNCTION h3_get_res_0_indexes() RETURNS SETOF h3index
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
    COMMENT ON FUNCTION h3_get_res_0_indexes() IS
'Returns all 122 resolution 0 indexes.';
