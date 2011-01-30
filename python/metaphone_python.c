/*
 * This is a port of the Double Metaphone based algorithm for brazilian 
 * portuguese, developed by Prefeitura Municipal de Varzea Paulista for PHP.
 *
 *
 * OBJECTIVES:
 *
 * As Double Metaphone, the ideia is to compute sound like strings, but
 * in this case, just for brazilian portuguese. Why? Because would be nice
 * to have generic tool for every language, but sometimes is not desirable or
 * either maintanable. Any changes could break the original metaphone code, and 
 * one way of avoiding this is introducing more "alternate" strings. Another one
 * is creating specifics metaphone functions for each language. That's my choice
 *
 * Information on using Double Metaphone can be found at
 *       http://www.codeproject.com/string/dmetaphone1.asp
 * and the original article describing it can be found at
 *       http://www.cuj.com/documents/s=8038/cuj0006philips/
 *
 * The original port for portuguese can be found here:
 * http://www2.varzeapaulista.sp.gov.br/metaphone/
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

  Copyright 2008, Carlos Costa Jordao <carlosjordao@gmail.com>.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification,
  are permitted provided that the following conditions are met:
  
  1. Redistributions of source code must retain the above copyright notice, this 
     list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.
  
  
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


***********************************************************************/


/* what kind of allocation functions will we use? */
#define META_MALLOC(v,n,t) (v = (t*)malloc(((n)*sizeof(t))))
#define META_REALLOC(v,n,t) (v = (t*)realloc((v),((n)*sizeof(t))))
#define META_FREE(x) free((x))

#include "../source/metaphone_ptbr.h"
#include <Python.h>
#include <locale.h>


static PyObject *
metaphoneptbr(PyObject *self, PyObject *args)
{
	int	alen;
	char	*code,
		*loc,
		*aptr;
	wchar_t *stringUCS;
	int mblength = 0, ret = 0;

	if (!PyArg_ParseTuple(args, "s", &aptr))
        	return NULL;


	alen = strlen(aptr);

        // converting to wide char...
        // 1.1 discovering strlength
        mblength = mbstowcs(NULL,aptr,0);
        if( mblength < 1 || mblength > sizeof(wchar_t)*alen ) {

		// Fixing locale part. This is very important for correct character conversion
		// if not set, will cause error with multibyte chars like 'ç'
		// just note that I only set locale for pt_BR when none is set by default.
		loc = setlocale(LC_CTYPE, NULL);
		if( !loc || !(*loc) || !strcmp(loc,"C") || !strcmp(loc,"POSIX") ) {
			if( !(loc=setlocale(LC_CTYPE,"pt_BR.UTF-8")) ) {
				return Py_BuildValue("s", "");
			}
		}

		// Trying again...
		mblength = mbstowcs(NULL,aptr,0);
		if( mblength < 1 || mblength > sizeof(wchar_t)*alen ) {
			return Py_BuildValue("s", "");
		}
        }

        // 1.2 converting
        META_MALLOC(stringUCS,mblength+1,wchar_t);
        ret = mbstowcs(stringUCS, aptr, mblength);
        if( ret < 0 ) {
                META_FREE(stringUCS);
        	return Py_BuildValue("s", "");
        }
        stringUCS[ret] = L'\0';
	//META_FREE(aptr);


	code = Metaphone_PTBR(stringUCS, MAX_METAPHONE_LENGTH);
	META_FREE(stringUCS);
	if (!code)
        	return Py_BuildValue("s", "");


	return Py_BuildValue("s", code);
}

static PyMethodDef MetaphoneMethods[] = {
    {"metaphoneptbr",  metaphoneptbr, METH_VARARGS, "Converts words to Portuguese sound like representantion"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
initmetaphoneptbr(void)
{
    (void) Py_InitModule("metaphoneptbr", MetaphoneMethods);
}

#include "../source/metaphone_ptbr.c"
