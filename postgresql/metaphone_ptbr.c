/*
 * This is a port of the Metaphone based algorithm for brazilian portuguese,
 * developed by Prefeitura Municipal de Varzea Paulista for PHP, for use in PostgreSQL.
 *
 */
/***************************** COPYRIGHT NOTICES ***********************

Some of this code is inspired on dmetaphone.c file, from postgresql contrib
tree directory, some is based on a PHP algorithm, which can be found here:
http://www2.varzeapaulista.sp.gov.br/metaphone/

The metaphone port is authored by Carlos Costa Jordao <carlosjordao@gmail.com>
and is covered under this copyright:

  Copyright 2008, Carlos C. Jordao <carlosjordao@gmail.com>.
  All rights reserved.

  This module is free software; you may redistribute it and/or
  modify it under the same terms as Perl itself.

***********************************************************************/

/* include these first, according to the docs */
#include "postgres.h"
#include "fmgr.h"
#if PG_MAJORVERSION_NUM >= 16 
#include "varatt.h"
#endif


#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

//#define NDEBUG

extern Datum metaphone_ptbr(PG_FUNCTION_ARGS);

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

#include "../../source/metaphone_ptbr.h"

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

	if (PG_ARGISNULL(0))
		PG_RETURN_NULL();

	if (!PG_ARGISNULL(1))
		maxmetalength = PG_GETARG_INT32(1);

	/* when second parameter is not set, is passed as zero. This leads to problems. dunno why */
	if (maxmetalength <= 0 )
		maxmetalength = MAX_METAPHONE_LENGTH;

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


#include "../../source/metaphone_ptbr.c"
