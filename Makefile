EXTENSION = h3
EXTVERSION = $(shell grep default_version $(EXTENSION).control | \
	sed -e "s/default_version[[:space:]]*=[[:space:]]*'\([^']*\)'/\1/")

INSTALL_SQL = sql/$(EXTENSION)--$(EXTVERSION).sql
UPDATES = $(wildcard sql/updates/${EXTENSION}--*--*.sql)
TESTS = $(wildcard test/sql/*.sql)

DATA = $(UPDATES) $(INSTALL_SQL) sql/$(EXTENSION)--upgradepath.sql
OBJS = $(patsubst %.c,%.o,$(wildcard src/*.c))
MODULE_big = $(EXTENSION)
SHLIB_LINK += -lh3

# Testing
REGRESS      = $(basename $(notdir $(TESTS)))
REGRESS_OPTS = --inputdir=test --outputdir=test --load-extension=postgis --load-extension=h3

all: $(INSTALL_SQL) sql/$(EXTENSION)--upgradepath.sql

sql/$(EXTENSION)--upgradepath.sql: $(sort $(UPDATES))
	cat $^ > $@

$(INSTALL_SQL): $(sort $(wildcard sql/install/*.sql))
	cat $^ > $@

installcheck: test/expected/install.out

test/expected/install.out: $(UPDATES)
	psql -c "CREATE DATABASE pg_regress;"
	psql -c "CREATE EXTENSION postgis;"
	psql -c "CREATE EXTENSION h3 VERSION 'upgradepath';"
	echo "\dx+ h3" > $@
	psql -c "\dx+ h3" >> $@
	psql -c "DROP DATABASE pg_regress;"

EXTRA_CLEAN += $(INSTALL_SQL) test/regression.diffs test/regression.out test/results

# PGXS boilerplate
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
