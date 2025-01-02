/*
 * Copyright 2024 Zacharias Knudsen
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

--@ availability: 4.2.0
CREATE OR REPLACE FUNCTION
    h3_polygon_to_cells_experimental(exterior polygon, holes polygon[], resolution integer DEFAULT 1, containment_mode text DEFAULT 'center') RETURNS SETOF h3index
AS 'h3' LANGUAGE C IMMUTABLE
-- intentionally NOT STRICT
CALLED ON NULL INPUT PARALLEL SAFE; COMMENT ON FUNCTION
    h3_polygon_to_cells_experimental(polygon, polygon[], integer, text)
IS 'Takes an exterior polygon [and a set of hole polygon] and returns the set of hexagons that best fit the structure.';
