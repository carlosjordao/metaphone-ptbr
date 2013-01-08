# burrowed from fuzzystrmatch
# tested with PostgreSQL 9.1

# with this, I can compile, install and uninstall from anywhere
# but at least the 'dev' package is needed.

MODULES = metaphone_ptbr
EXTENSION = metaphone_ptbr
DATA = metaphone_ptbr--1.34.sql

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

clean:
	rm -f *.o *.so

