/*
 * Copyright 2018-2020 Bytes & Brains
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

#ifndef PGH3_H
#define PGH3_H

#include <h3api.h> // Main H3 include

typedef struct
{
	H3Index    *indices;
	int		   *distances;
}	hexDistanceTuple;

#define MAX_H3_RES 15

/*	configure extension version in makefile */
#define EXTVERSION "@EXTVERSION@"

/*
 * DatumGetH3Index
 *		Returns H3 index value of a datum.
 *
 * Note: this macro hides whether h3 index is pass by value or by reference.
 */

#ifdef USE_FLOAT8_BYVAL
#define DatumGetH3Index(X) ((H3Index) (X))
#else
#define DatumGetH3Index(X) (* ((H3Index *) DatumGetPointer(X)))
#endif

/*
 * H3IndexGetDatum
 *		Returns datum representation for an H3 index.
 *
 * Note: if H3 index is pass by reference, this function returns a reference
 * to palloc'd space.
 */

#ifdef USE_FLOAT8_BYVAL
#define H3IndexGetDatum(X) ((Datum) (X))
#else
#define H3IndexGetDatum(X) Int64GetDatum((int64) (X))
#endif

/* Macros for fetching arguments and returning results of h3 index type */

#define PG_GETARG_H3INDEX(n) DatumGetH3Index(PG_GETARG_DATUM(n))
#define PG_RETURN_H3INDEX(x) return H3IndexGetDatum(x)

/* GiST macros */

#define DatumGetH3IndexP(x) (H3Index *) DatumGetPointer(x)
#define H3IndexPGetDatum(x) PointerGetDatum(x)

/*	helper functions to return sets from user fctx */
Datum		srf_return_h3_indexes_from_user_fctx(PG_FUNCTION_ARGS);
Datum		srf_return_h3_index_distances_from_user_fctx(PG_FUNCTION_ARGS);

/*	macros to pass on fcinfo to above helpers */
#define SRF_RETURN_H3_INDEXES_FROM_USER_FCTX() \
	return srf_return_h3_indexes_from_user_fctx(fcinfo)
#define SRF_RETURN_H3_INDEX_DISTANCES_FROM_USER_FCTX() \
	return srf_return_h3_index_distances_from_user_fctx(fcinfo)

#define ASSERT(condition, code, msg, ...)  \
	if (0 == (condition)) ereport(ERROR, ( \
		errcode(code),					   \
		errmsg(msg, ##__VA_ARGS__)		   \
	))

#define ASSERT_EXTERNAL(condition, msg, ...) \
	ASSERT(condition, ERRCODE_EXTERNAL_ROUTINE_EXCEPTION, msg, ##__VA_ARGS__)

#define ENSURE_TYPEFUNC_COMPOSITE(x)				   \
	ASSERT(											   \
		x == TYPEFUNC_COMPOSITE,					   \
		ERRCODE_INVALID_PARAMETER_VALUE,			   \
		"Function returning record called in context " \
		"that cannot accept type record"			   \
	)

#define DEBUG(msg, ...)			   \
	ereport(ERROR, (			   \
		errmsg(msg, ##__VA_ARGS__) \
	))

#define DEBUG_H3INDEX(h3index) DEBUG("index: %lx", h3index)

#endif
