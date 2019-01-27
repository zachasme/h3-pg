EXTENSION = h3
EXTVERSION = $(shell grep default_version $(EXTENSION).control | \
	sed -e "s/default_version[[:space:]]*=[[:space:]]*'\([^']*\)'/\1/")

LIBH3_REQUIRED_MAJOR = 3
LIBH3_REQUIRED_MINOR = 4
LIBH3_REQUIRED_PATCH = 0
LIBH3_VERSION = v${LIBH3_REQUIRED_MAJOR}.${LIBH3_REQUIRED_MINOR}.${LIBH3_REQUIRED_PATCH}
LIBH3_DIR = libh3-${LIBH3_VERSION}

INSTALL_FILES = $(wildcard sql/install/*.sql)
UPDATE_FILES = $(wildcard sql/updates/*.sql)
FULLINSTALL_SQL = sql/$(EXTENSION)--$(EXTVERSION).sql
UPDATETEST_SQL = sql/$(EXTENSION)--updatetest.sql
TESTS = $(wildcard test/sql/*.sql)

DATA = $(UPDATES) $(FULLINSTALL_SQL) $(UPDATETEST_SQL)
OBJS = $(patsubst %.c,%.o,$(wildcard src/*.c))
MODULE_big = $(EXTENSION)
SHLIB_LINK += -lh3 -L${LIBH3_DIR}/install/lib
PG_CPPFLAGS += -I${LIBH3_DIR}/install/include

# testing
REGRESS      = $(basename $(notdir $(TESTS)))
REGRESS_OPTS = --inputdir=test --outputdir=test --load-extension=postgis --load-extension=h3

# build and compile h3 as static lib if not already installed
${LIBH3_DIR}:
	git clone --branch ${LIBH3_VERSION} --depth 1 https://github.com/uber/h3 ${LIBH3_DIR}
h3.a: ${LIBH3_DIR}
	$(info Building H3 locally and linking statically)
	cd ${LIBH3_DIR} && cmake \
			-DCMAKE_C_FLAGS=-fPIC \
			-DCMAKE_INSTALL_PREFIX:PATH=./install \
			-DBUILD_TESTING=OFF \
			-DENABLE_COVERAGE=OFF \
			-DENABLE_DOCS=OFF \
			-DENABLE_FORMAT=OFF \
			-DENABLE_LINTING=OFF \
			.
	cmake --build ${LIBH3_DIR} --target install
$(OBJS): h3.a

# generate header file
src/extension.h: src/extension.in.h
	sed -e 's/@EXTVERSION@/${EXTVERSION}/g' \
		-e 's/@LIBH3_REQUIRED_MAJOR@/${LIBH3_REQUIRED_MAJOR}/g' \
		-e 's/@LIBH3_REQUIRED_MINOR@/${LIBH3_REQUIRED_MINOR}/g' \
		-e 's/@LIBH3_REQUIRED_PATCH@/${LIBH3_REQUIRED_PATCH}/g' \
		$< > $@
$(OBJS): src/extension.h

# generate full install sql file
$(FULLINSTALL_SQL): $(sort $(INSTALL_FILES))
	cat $^ > $@
install: $(FULLINSTALL_SQL)

# rules for testing the update path against full install
$(UPDATETEST_SQL): $(sort $(UPDATE_FILES))
	cat $^ > $@
test/expected/install.out: $(UPDATE_FILES)
	psql -c "DROP DATABASE IF EXISTS pg_regress;"
	psql -c "CREATE DATABASE pg_regress;"
	psql -d pg_regress -c "CREATE EXTENSION postgis;"
	psql -d pg_regress -c "CREATE EXTENSION h3 VERSION 'updatetest';"
	echo "\df h3*" > $@
	psql -d pg_regress -c "\df h3*" >> $@
	psql -c "DROP DATABASE pg_regress;"
install: $(UPDATETEST_SQL)
installcheck: test/expected/install.out

# zip up for distribution
distribute: clean
	git archive --format zip --prefix=h3-${EXTVERSION}/ --output h3-${EXTVERSION}.zip master

# cleanup
EXTRA_CLEAN += \
	${LIBH3_DIR} \
	$(FULLINSTALL_SQL) \
	$(UPDATETEST_SQL) \
	src/extension.h \
	test/expected/install.out \
	test/regression.diffs test/regression.out test/results \
	h3-*.zip

# PGXS boilerplate
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
