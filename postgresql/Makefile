# burrowed from fuzzystrmatch
# tested with PostgreSQL 8.3

# with this, I can compile, install and uninstall from anywhere
# but at least the 'dev' package is needed.
USE_PGXS=1
#PATH=/sbin:/usr/sbin:/usr/local/sbin:/bin:/usr/bin:/usr/local/bin:.:$(PATH)

MODULE_big = metaphone_ptbr
OBJS = metaphone_pgmain.o
DATA_built = metaphone_ptbr.sql
DATA = uninstall_metaphone_ptbr.sql

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/metaphoneptbr
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
