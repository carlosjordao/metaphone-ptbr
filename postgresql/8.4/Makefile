# burrowed from fuzzystrmatch
# tested with PostgreSQL 8.3

# with this, I can compile, install and uninstall from anywhere
# but at least the 'dev' package is needed.
#PATH=/sbin:/usr/sbin:/usr/local/sbin:/bin:/usr/bin:/usr/local/bin:.:$(PATH)

MODULES = metaphone_ptbr
EXTENSION = metaphone_ptbr
#OBJS = metaphone_pgmain.o
DATA_built = metaphone_ptbr.sql
DATA = uninstall_metaphone_ptbr.sql

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

clean:
	rm -f *.o *.so

