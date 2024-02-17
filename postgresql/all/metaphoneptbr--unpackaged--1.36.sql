/* $PostgreSQL: pgsql/contrib/fuzzystrmatch/uninstall_fuzzystrmatch.sql,v 1.3 2007/11/13 04:24:28 momjian Exp $ */

-- Adjust this setting to control where the objects get dropped.
SET search_path = public;

DROP FUNCTION metaphone_ptbr (text);

