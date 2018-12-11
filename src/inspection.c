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

#include <h3/h3api.h> // Main H3 include
#include "h3-pg.h"

// Returns the resolution of the index
PG_FUNCTION_INFO_V1(h3_h3_get_resolution);
Datum h3_h3_get_resolution(PG_FUNCTION_ARGS)
{
    H3Index *hex = PG_GETARG_H3_INDEX_P(0);
    int resolution = h3GetResolution(*hex);
    PG_RETURN_INT32(resolution);
}

// Returns the base cell number of the index
PG_FUNCTION_INFO_V1(h3_h3_get_base_cell);
Datum h3_h3_get_base_cell(PG_FUNCTION_ARGS)
{
    H3Index *hex = PG_GETARG_H3_INDEX_P(0);
    int base_cell_number = h3GetBaseCell(*hex);
    PG_RETURN_INT32(base_cell_number);
}

// Converts the string representation to internal representation
PG_FUNCTION_INFO_V1(h3_string_to_h3);
Datum h3_string_to_h3(PG_FUNCTION_ARGS)
{
    H3Index *hex;
    char *str;

    str = PG_GETARG_CSTRING(0);
    hex = palloc(sizeof(H3Index));
    *hex = stringToH3(str);

    PG_RETURN_H3_INDEX_P(hex);
}

// Converts the internal representation of the index to the string
// representation
PG_FUNCTION_INFO_V1(h3_h3_to_string);
Datum h3_h3_to_string(PG_FUNCTION_ARGS)
{
    H3Index *hex = PG_GETARG_H3_INDEX_P(0);
    char *str = palloc(17 * sizeof(char));
    h3ToString(*hex, str, 17);

    PG_RETURN_CSTRING(str);
}

// Returns true if this is a valid H3 index
PG_FUNCTION_INFO_V1(h3_h3_is_valid);
Datum h3_h3_is_valid(PG_FUNCTION_ARGS)
{
    H3Index *hex = PG_GETARG_H3_INDEX_P(0);
    bool isValid = h3IsValid(*hex);
    PG_RETURN_BOOL(isValid);
}

// Returns true if this index has a resolution with Class III orientation
PG_FUNCTION_INFO_V1(h3_h3_is_res_class_iii);
Datum h3_h3_is_res_class_iii(PG_FUNCTION_ARGS)
{
    H3Index *hex = PG_GETARG_H3_INDEX_P(0);
    bool isResClassIII = h3IsResClassIII(*hex);
    PG_RETURN_BOOL(isResClassIII);
}

// Returns true if this hex represents a pentagonal cell
PG_FUNCTION_INFO_V1(h3_h3_is_pentagon);
Datum h3_h3_is_pentagon(PG_FUNCTION_ARGS)
{
    H3Index *hex = PG_GETARG_H3_INDEX_P(0);
    bool isPentagon = h3IsPentagon(*hex);
    PG_RETURN_BOOL(isPentagon);
}
