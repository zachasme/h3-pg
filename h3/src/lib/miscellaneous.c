/*
 * Copyright 2018-2022 Bytes & Brains
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *	   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <postgres.h> // Datum, etc.
#include <fmgr.h>	  // PG_FUNCTION_ARGS, etc.
#include <funcapi.h>  // Definitions for functions which return sets
#include <utils/geo_decls.h> // making native points
#include <utils/builtins.h> // making native points

#include <h3api.h> // Main H3 include
#include "extension.h"

PG_FUNCTION_INFO_V1(h3_get_hexagon_area_avg);
PG_FUNCTION_INFO_V1(h3_cell_area);
PG_FUNCTION_INFO_V1(h3_get_hexagon_edge_length_avg);
PG_FUNCTION_INFO_V1(h3_edge_length);
PG_FUNCTION_INFO_V1(h3_get_num_cells);
PG_FUNCTION_INFO_V1(h3_get_res_0_cells);
PG_FUNCTION_INFO_V1(h3_get_pentagons);
PG_FUNCTION_INFO_V1(h3_great_circle_distance);

/* Average hexagon area in square (kilo)meters at the given resolution */
Datum
h3_get_hexagon_area_avg(PG_FUNCTION_ARGS)
{
	int			resolution = PG_GETARG_INT32(0);
	char	   *unit = text_to_cstring(PG_GETARG_TEXT_PP(1));
	double		area;
	H3Error		error;

	if (strcmp(unit, "km") == 0)
		error = getHexagonAreaAvgKm2(resolution, &area);
	else if (strcmp(unit, "m") == 0)
		error = getHexagonAreaAvgM2(resolution, &area);
	else
		ASSERT(0, ERRCODE_INVALID_PARAMETER_VALUE, "Unit must be m or km.");

	H3_ERROR(error, "getHexagonAreaAvgM2");

	PG_RETURN_FLOAT8(area);
}

/* Exact area for a specific cell (hexagon or pentagon) */
Datum
h3_cell_area(PG_FUNCTION_ARGS)
{
	H3Index		cell = PG_GETARG_H3INDEX(0);
	char	   *unit = text_to_cstring(PG_GETARG_TEXT_PP(1));
	double		area;
	H3Error		error;

	if (strcmp(unit, "rads^2") == 0)
	{
		error = cellAreaRads2(cell, &area);
		H3_ERROR(error, "cellAreaRads2");
	}
	else if (strcmp(unit, "km^2") == 0)
	{
		error = cellAreaKm2(cell, &area);
		H3_ERROR(error, "cellAreaKm2");
	}
	else if (strcmp(unit, "m^2") == 0)
	{
		error = cellAreaM2(cell, &area);
		H3_ERROR(error, "cellAreaM2");
	}
	else
	{
		ASSERT(0, ERRCODE_INVALID_PARAMETER_VALUE, "Unit must be m^2, km^2 or rads^2.");
	}

	PG_RETURN_FLOAT8(area);
}

/* Average hexagon edge length in (kilo)meters at the given resolution */
Datum
h3_get_hexagon_edge_length_avg(PG_FUNCTION_ARGS)
{
	int			resolution = PG_GETARG_INT32(0);
	char	   *unit = text_to_cstring(PG_GETARG_TEXT_PP(1));
	double		length;
	H3Error		error;

	if (strcmp(unit, "km") == 0)
	{
		error = getHexagonEdgeLengthAvgKm(resolution, &length);
		H3_ERROR(error, "getHexagonEdgeLengthAvgKm");
	}
	else if (strcmp(unit, "m") == 0)
	{
		error = getHexagonEdgeLengthAvgM(resolution, &length);
		H3_ERROR(error, "getHexagonEdgeLengthAvgM");
	}
	else
	{
		ASSERT(0, ERRCODE_INVALID_PARAMETER_VALUE, "Unit must be m or km.");
	}


	PG_RETURN_FLOAT8(length);
}

/* Exact length for a specific unidirectional edge */
Datum
h3_edge_length(PG_FUNCTION_ARGS)
{
	H3Index		edge = PG_GETARG_H3INDEX(0);
	char	   *unit = text_to_cstring(PG_GETARG_TEXT_PP(1));
	double		length;
	H3Error		error;

	if (strcmp(unit, "rads") == 0)
	{
		error = edgeLengthRads(edge, &length);
		H3_ERROR(error, "edgeLengthRads");
	}
	else if (strcmp(unit, "km") == 0)
	{
		error = edgeLengthKm(edge, &length);
		H3_ERROR(error, "edgeLengthKm");
	}
	else if (strcmp(unit, "m") == 0)
	{
		error = edgeLengthM(edge, &length);
		H3_ERROR(error, "edgeLengthM");
	}
	else
	{
		ASSERT(0, ERRCODE_INVALID_PARAMETER_VALUE, "Unit must be m, km or rads.");
	}

	PG_RETURN_FLOAT8(length);
}

/* Number of unique H3 indexes at the given resolution */
Datum
h3_get_num_cells(PG_FUNCTION_ARGS)
{
	int64_t		cells;
	int			resolution = PG_GETARG_INT32(0);
	H3Error		error = getNumCells(resolution, &cells);

	H3_ERROR(error, "getNumCells");

	PG_RETURN_INT64(cells);
}

/* Provides all resolution 0 indexes */
Datum
h3_get_res_0_cells(PG_FUNCTION_ARGS)
{
	if (SRF_IS_FIRSTCALL())
	{
		FuncCallContext *funcctx = SRF_FIRSTCALL_INIT();
		MemoryContext oldcontext =
		MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

		H3Error		error;
		int			count = res0CellCount();
		H3Index    *indexes = palloc(count * sizeof(H3Index));

		error = getRes0Cells(indexes);
		H3_ERROR(error, "getRes0Cells");

		funcctx->user_fctx = indexes;
		funcctx->max_calls = count;
		MemoryContextSwitchTo(oldcontext);
	}

	SRF_RETURN_H3_INDEXES_FROM_USER_FCTX();
}

/* All the pentagon H3 indexes at the specified resolution */
Datum
h3_get_pentagons(PG_FUNCTION_ARGS)
{
	if (SRF_IS_FIRSTCALL())
	{
		FuncCallContext *funcctx = SRF_FIRSTCALL_INIT();
		MemoryContext oldcontext =
		MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

		H3Error		error;
		int			resolution = PG_GETARG_INT32(0);
		int			count = pentagonCount();
		H3Index    *indexes = palloc(count * sizeof(H3Index));

		error = getPentagons(resolution, indexes);
		H3_ERROR(error, "getPentagons");

		funcctx->user_fctx = indexes;
		funcctx->max_calls = count;
		MemoryContextSwitchTo(oldcontext);
	}

	SRF_RETURN_H3_INDEXES_FROM_USER_FCTX();
}

/* The great circle distance in radians between two spherical coordinates */
Datum
h3_great_circle_distance(PG_FUNCTION_ARGS)
{
	Point	   *aPoint = PG_GETARG_POINT_P(0);
	Point	   *bPoint = PG_GETARG_POINT_P(1);
	char	   *unit = text_to_cstring(PG_GETARG_TEXT_PP(2));

	LatLng		a;
	LatLng		b;
	double		distance;

	a.lng = degsToRads(aPoint->x);
	a.lat = degsToRads(aPoint->y);
	b.lng = degsToRads(bPoint->x);
	b.lat = degsToRads(bPoint->y);

	if (strcmp(unit, "rads") == 0)
		distance = greatCircleDistanceRads(&a, &b);
	else if (strcmp(unit, "km") == 0)
		distance = greatCircleDistanceKm(&a, &b);
	else if (strcmp(unit, "m") == 0)
		distance = greatCircleDistanceM(&a, &b);
	else
		ASSERT(0, ERRCODE_INVALID_PARAMETER_VALUE, "Unit must be m, km or rads.");

	PG_RETURN_FLOAT8(distance);
}
