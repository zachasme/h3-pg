EXTENSION = h3
EXTVERSION = $(shell grep default_version $(EXTENSION).control | \
	sed -e "s/default_version[[:space:]]*=[[:space:]]*'\([^']*\)'/\1/")

UPDATES = $(wildcard sql/updates/${EXTENSION}--*--*.sql)

DATA = $(UPDATES) sql/$(EXTENSION)--$(EXTVERSION).sql
OBJS = $(patsubst %.c,%.o,$(wildcard src/*.c))
MODULE_big = $(EXTENSION)
SHLIB_LINK += -lh3

# Testing
REGRESS      = $(basename $(notdir $(wildcard test/sql/*.sql)))
REGRESS_OPTS = --inputdir=test --outputdir=test --load-extension=postgis --load-extension=h3

all: sql/$(EXTENSION)--$(EXTVERSION).sql

sql/$(EXTENSION)--$(EXTVERSION).sql: $(sort $(UPDATES))
	cat $^ > $@

EXTRA_CLEAN += sql/$(EXTENSION)--$(EXTVERSION).sql test/regression.diffs test/regression.out test/results

# PGXS boilerplate
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
