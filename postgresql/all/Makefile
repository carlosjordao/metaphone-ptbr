# burrowed from fuzzystrmatch
# tested with PostgreSQL 9.1

# with this, I can compile, install and uninstall from anywhere
# but at least the 'dev' package is needed.

MODULES = metaphoneptbr
EXTENSION = metaphoneptbr
DATA = metaphoneptbr--1.36.sql
CFLAGS= -Wimplicit-fallthrough=0

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

clean:
	rm -f *.o *.so

