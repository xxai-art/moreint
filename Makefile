PG_CONFIG = pg_config

pg_version := $(word 2,$(shell $(PG_CONFIG) --version))
indexonlyscan_supported = $(filter-out 6.% 7.% 8.% 9.0% 9.1%,$(pg_version))

# Disable index-only scans here so that the regression test output is
# the same in versions that don't support it.
ifneq (,$(indexonlyscan_supported))
export PGOPTIONS = -c enable_indexonlyscan=off
endif

extension_version = $(shell cat version)

EXTENSION = moreint
MODULE_big = moreint
OBJS = aggregates.o hash.o hex.o inout.o magic.o misc.o operators.o
DATA_built = moreint--$(extension_version).sql

REGRESS = init hash hex operators misc drop
REGRESS_OPTS = --inputdir=test

EXTRA_CLEAN += operators.c operators.sql test/sql/operators.sql

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

moreint--$(extension_version).sql: moreint.sql hash.sql hex.sql operators.sql
	cat $^ >$@

PYTHON ?= python

operators.c operators.sql test/sql/operators.sql: generate.py
	./generate.py $(MAJORVERSION)

python-check: generate.py
	pep8 $^
	pyflakes $^
	pylint $^
