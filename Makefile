EXTENSION = h3
EXTVERSION = $(shell grep default_version $(EXTENSION).control | \
	sed -e "s/default_version[[:space:]]*=[[:space:]]*'\([^']*\)'/\1/")

INSTALL_FILES = $(wildcard sql/install/*.sql)
UPDATE_FILES = $(wildcard sql/updates/*.sql)
FULLINSTALL_SQL = sql/$(EXTENSION)--$(EXTVERSION).sql
UPDATETEST_SQL = sql/$(EXTENSION)--updatetest.sql
TESTS = $(wildcard test/sql/*.sql)

DATA = $(UPDATES) $(FULLINSTALL_SQL) $(UPDATETEST_SQL)
OBJS = $(patsubst %.c,%.o,$(wildcard src/*.c))
MODULE_big = $(EXTENSION)
SHLIB_LINK += -lh3

# Testing
REGRESS      = $(basename $(notdir $(TESTS)))
REGRESS_OPTS = --inputdir=test --outputdir=test --load-extension=postgis --load-extension=h3

# Generate header file
submake: src/extension.h
src/extension.h: src/extension.h.in
	sed s/@EXTVERSION@/${EXTVERSION}/g $< > $@

all: $(FULLINSTALL_SQL) $(UPDATETEST_SQL)

$(FULLINSTALL_SQL): $(sort $(INSTALL_FILES))
	cat $^ > $@

$(UPDATETEST_SQL): $(sort $(UPDATE_FILES))
	cat $^ > $@

installcheck: test/expected/install.out

test/expected/install.out: $(UPDATE_FILES)
	psql -c "CREATE DATABASE pg_regress;"
	psql -c "CREATE EXTENSION postgis;"
	psql -c "CREATE EXTENSION h3 VERSION 'updatetest';"
	echo "\df h3*" > $@
	psql -c "\df h3*" >> $@
	psql -c "DROP DATABASE pg_regress;"

EXTRA_CLEAN += $(FULLINSTALL_SQL) $(UPDATETEST_SQL) test/regression.diffs test/regression.out test/results

# PGXS boilerplate
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
