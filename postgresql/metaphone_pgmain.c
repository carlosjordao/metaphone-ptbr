/*
 * This is a port of the Double Metaphone based algorithm for brazilian portuguese,
 * developed by Prefeitura Municipal de Varzea Paulista for PHP, for use in PostgreSQL.
 *
 * $PostgreSQL: pgsql/contrib/fuzzystrmatch/dmetaphone_ptbr.c,v 1.11 2007/02/27 23:48:05 tgl Exp $
 *
 * As Double Metaphone, the ideia is to compute sound like strings, but
 * in this case, just for brazilian portuguese. Why? Because would be nice
 * to have generic tool for every language, but sometimes is not desirable or
 * either maintanable. Any changes could break the code, and one way of 
 * avoiding this is introducing more "alternate" strings. Another one is
 * creating specifics metaphone functions.
 *
 * Information on using Double Metaphone can be found at
 *	 http://www.codeproject.com/string/dmetaphone1.asp
 * and the original article describing it can be found at
 *	 http://www.cuj.com/documents/s=8038/cuj0006philips/
 *
 * For PostgreSQL we provide 2 functions - one for the primary and one for
 * the alternate. That way the functions are pure text->text mappings that
 * are useful in functional indexes. These are 'dmetaphone' for the
 * primary and 'dmetaphone_alt' for the alternate.
 *
 * Assuming that dmetaphone.so is in $libdir, the SQL to set up the
 * functions looks like this:
 *
 * CREATE FUNCTION dmetaphone_ptbr (text) RETURNS text
 *	  LANGUAGE C IMMUTABLE STRICT
 *	  AS '$libdir/dmetaphone', 'dmetaphone';
 *
 *
 * Note that you have to declare the functions IMMUTABLE if you want to
 * use them in functional indexes, and you have to declare them as STRICT
 * as they do not check for NULL input, and will segfault if given NULL input.
 * (See below for alternative ) Declaring them as STRICT means PostgreSQL
 * will never call them with NULL, but instead assume the result is NULL,
 * which is what we (I) want.
 *
 * Alternatively, compile with -DDMETAPHONE_NOSTRICT and the functions
 * will detect NULL input and return NULL. The you don't have to declare them
 * as STRICT.
 *
 *
 * BUGS: possible there is some, on skips parts. That could confuse last_char
 * variable, so some cases may be desconsidered. It's safe, but not correct.
 */


/***************************** COPYRIGHT NOTICES ***********************

Some of this code is based on dmetaphone.c file, from postgresql contrib
tree directory, some is based on a PHP algorithm, which can be found here:
http://www2.varzeapaulista.sp.gov.br/metaphone/

The original double metaphone code bears this copyright notice:

  Copyright 2000, Maurice Aubrey <maurice@hevanet.com>.
  All rights reserved.

  This code is based heavily on the C++ implementation by
  Lawrence Philips and incorporates several bug fixes courtesy
  of Kevin Atkinson <kevina@users.sourceforge.net>.

  This module is free software; you may redistribute it and/or
  modify it under the same terms as Perl itself.



The original code is authored by Andrew Dunstan <amdunstan@ncshp.org>, and
<andrew@dunslane.net> and is covered this copyright:

  Copyright 2003, North Carolina State Highway Patrol.
  All rights reserved.

  Permission to use, copy, modify, and distribute this software and its
  documentation for any purpose, without fee, and without a written agreement
  is hereby granted, provided that the above copyright notice and this
  paragraph and the following two paragraphs appear in all copies.

  IN NO EVENT SHALL THE NORTH CAROLINA STATE HIGHWAY PATROL BE LIABLE TO ANY
  PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES,
  INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS
  DOCUMENTATION, EVEN IF THE NORTH CAROLINA STATE HIGHWAY PATROL HAS BEEN
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  THE NORTH CAROLINA STATE HIGHWAY PATROL SPECIFICALLY DISCLAIMS ANY
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED
  HEREUNDER IS ON AN "AS IS" BASIS, AND THE NORTH CAROLINA STATE HIGHWAY PATROL
  HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
  MODIFICATIONS.



The metaphone port is authored by Carlos Costa Jordao <carlosjordao@gmail.com>
and is covered under this copyright:

  Copyright 2008, Carlos C. Jordao <carlosjordao@gmail.com>.
  All rights reserved.

  This module is free software; you may redistribute it and/or
  modify it under the same terms as Perl itself.

***********************************************************************/





/* include these first, according to the docs */
#ifndef PT_METAPHONE_MAIN

#include "postgres.h"
#include "fmgr.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

/* turn off assertions for embedded function */
//#define NDEBUG
#endif


/* prototype for the main function we got from the perl module */

#ifndef DMETAPHONE_MAIN

extern Datum metaphone_ptbr(PG_FUNCTION_ARGS);

/*
 * The PostgreSQL visible dmetaphone function.
 *
 */

#include "../source/metaphone_ptbr.h"
#define META_MALLOC(v,n,t) \
		  (v = (t*)palloc(((n)*sizeof(t))))

#define META_REALLOC(v,n,t) \
					  (v = (t*)repalloc((v),((n)*sizeof(t))))

/*
 * Don't do pfree - it seems to cause a segv sometimes - which might have just
 * been caused by reloading the module in development.
 * So we rely on context cleanup - Tom Lane says pfree shouldn't be necessary
 * in a case like this.
 */

#define META_FREE(x)			 pfree((x)) 

PG_FUNCTION_INFO_V1(metaphone_ptbr);

Datum
metaphone_ptbr(PG_FUNCTION_ARGS)
{
	text	*arg,
		*result = NULL;
	int	alen,
		rsize,
		maxmetalength = MAX_METAPHONE_LENGTH;
	char	*code,
		*aptr;
	wchar_t *stringUCS;
	int mblength = 0, ret = 0;

/* #ifdef PT_METAPHONE_NOSTRICT */
	if (PG_ARGISNULL(0))
		PG_RETURN_NULL();

	if (!PG_ARGISNULL(1))
		maxmetalength = PG_GETARG_INT32(1);
/* #endif */
	arg = PG_GETARG_TEXT_P(0);
	alen = VARSIZE(arg) - VARHDRSZ;

	/* no valid string - just quit */
	if (alen <= 0)
		PG_RETURN_NULL();
	

	/*
	 * Postgres' string values might not have trailing nuls. The VARSIZE will
	 * not include the nul in any case so we copy things out and add a
	 * trailing nul. When we copy back we ignore the nul (and we don't make
	 * space for it).
	 */


	META_MALLOC(aptr,alen + 1, char);
	memcpy(aptr, VARDATA(arg), alen);
	aptr[alen] = '\0';


        // converting to wide char...
        // 1.1 discovering strlength
        mblength = mbstowcs(NULL,aptr,0);
        if( mblength < 1 || mblength > sizeof(wchar_t)*alen ) {
                PG_RETURN_TEXT_P("");
        }

        // 1.2 converting
        META_MALLOC(stringUCS,mblength+1,wchar_t);
        ret = mbstowcs(stringUCS, aptr, mblength);
        if( ret < 0 ) {
                META_FREE(stringUCS);
                PG_RETURN_TEXT_P("");
        }
        stringUCS[ret] = L'\0';
	META_FREE(aptr);


	code = Metaphone_PTBR(stringUCS, maxmetalength);
	META_FREE(stringUCS);
	if (!code)
                PG_RETURN_TEXT_P("");

	rsize = strlen(code);

	result = (text *) palloc(rsize+VARHDRSZ);
	memcpy(VARDATA(result), code, rsize);
	pfree(code);
	SET_VARSIZE(result, VARHDRSZ+rsize);
	PG_RETURN_TEXT_P(result);
}



#endif

#include "../source/metaphone_ptbr.c"
