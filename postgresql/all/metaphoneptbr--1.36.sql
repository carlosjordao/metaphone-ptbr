/* $PostgreSQL: pgsql/contrib/fuzzystrmatch/fuzzystrmatch.sql.in,v 1.9 2007/11/13 04:24:28 momjian Exp $ */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION pair" to load this file. \quit


-- Adjust this setting to control where the objects get created.
--SET search_path = public;

CREATE OR REPLACE FUNCTION metaphone_ptbr (text) RETURNS text 
AS 'MODULE_PATHNAME', 'metaphone_ptbr'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION metaphone_ptbr (text, int) RETURNS text 
--AS 'MODULE_PATHNAME', 'metaphone_ptbr'
AS 'MODULE_PATHNAME', 'metaphone_ptbr'
LANGUAGE C IMMUTABLE STRICT;

