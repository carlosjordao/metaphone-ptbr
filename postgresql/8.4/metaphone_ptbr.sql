/* $PostgreSQL: pgsql/contrib/fuzzystrmatch/fuzzystrmatch.sql.in,v 1.9 2007/11/13 04:24:28 momjian Exp $ */

-- Adjust this setting to control where the objects get created.
SET search_path = public;

CREATE OR REPLACE FUNCTION metaphone_ptbr (text) RETURNS text 
AS '$libdir/metaphone_ptbr', 'metaphone_ptbr'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION metaphone_ptbr (text, int) RETURNS text 
AS '$libdir/metaphone_ptbr', 'metaphone_ptbr'
LANGUAGE C IMMUTABLE STRICT;


