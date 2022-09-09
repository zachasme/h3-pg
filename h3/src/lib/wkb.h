#ifndef PGH3_WKB_H
#define PGH3_WKB_H

#include <postgres.h>
#include <h3api.h>
#include <fmgr.h>

bytea*
boundary_array_to_wkb(const CellBoundary *boundaries, size_t num);

bytea*
boundary_to_wkb(const CellBoundary *boundary);

#endif
