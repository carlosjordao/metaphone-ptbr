/* ***************************************
  This is a port of the Double Metaphone based algorithm for brazilian 
  portuguese, developed by Prefeitura Municipal de Varzea Paulista for PHP.
 
 
  OBJECTIVES:
 
  As Double Metaphone, the ideia is to compute sound like strings, but
  in this case, just for brazilian portuguese. Why? Because would be nice
  to have generic tool for every language, but sometimes is not desirable or
  either maintanable. Any changes could break the original metaphone code, and 
  one way of avoiding this is introducing more "alternate" strings. Another one
  is creating specifics metaphone functions for each language. That's my choice
 
  Information on using Double Metaphone can be found at
        http://www.codeproject.com/string/dmetaphone1.asp
  and the original article describing it can be found at
        http://www.cuj.com/documents/s=8038/cuj0006philips/
 
  The original port for portuguese can be found here:
  http://www2.varzeapaulista.sp.gov.br/metaphone/
 

 * COPYRIGHT *

 
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

*/


#ifndef METAPHONE_PTBR_H
#define METAPHONE_PTBR_H

#ifndef _POSIX_C_SOURCE
// for python code
#include <stdio.h>
#endif
 #include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <unistd.h>
#include <wchar.h>
#include <wctype.h>
#include <fcntl.h>
#include <locale.h>

/* 50 should be enough for long names - based on statistical analysis 
 * but for single words you should use something like 3 as max length 
 * for the result metaphone string. That would lead you to best scenario.
 */
#define MAX_METAPHONE_LENGTH 50

/* this one will help detect, in special inside MakeUpperAndClean() */
#define DOUBLED_CHAR(i) ( i && *i && ((*i) == (*(i+1))) )
/* Is this character outside of a word? */
#define WORD_EDGE(c) (c == L'\0' || isspace(c) || c==separator)


/* this typedef was originally in the perl module's .h file */

typedef struct
{
        char	*str;
        int	length;
        int	bufsize;
        int	free_string_on_destroy;
} metastring;


static metastring * NewMetaString(char *init_str);
static void DestroyMetaString(metastring * s);
static void IncreaseBuffer(metastring * s, int chars_needed);
static wchar_t* MakeUpperAndClean(wchar_t * s);
static wchar_t GetAt(wchar_t* s, int pos);
static void MetaphAdd(metastring * s, char *new_str);
static void MetaphAddChr(metastring * s, char new_str);
static int isVowel(char chr);
static char * Metaphone_PTBR(const wchar_t *str, const int max_length);
/* word bound char, as demanded. This is good when applied in long names */
static char * Metaphone_PTBR_s(const wchar_t *str, const int max_length, const wchar_t separator);



/* use the standard malloc library when standalone */

#ifdef PT_METAPHONE_MAIN

#define META_MALLOC(v,n,t) (v = (t*)malloc(((n)*sizeof(t))))
#define META_REALLOC(v,n,t) (v = (t*)realloc((v),((n)*sizeof(t))))
#define META_FREE(x) free((x))

#endif


#endif
