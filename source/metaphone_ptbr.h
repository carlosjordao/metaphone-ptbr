/* ***************************************
  Copyright 2008-2017, Carlos Costa Jordao <carlosjordao@gmail.com>.
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

#include <ctype.h>
#include <stdio.h>
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
 * but for single words you should use something like 3 or 4 as max length
 * for the result metaphone string. That would lead you to best scenario.
 */
#define MAX_METAPHONE_LENGTH 4

/* this one will help detect, in special inside MakeUpperAndClean() */
#define DOUBLED_CHAR(i) ((*i) == (*(i-1)))
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


metastring * NewMetaString(char *init_str);
void DestroyMetaString(metastring * s);
void IncreaseBuffer(metastring * s, int chars_needed);
wchar_t* MakeUpperAndClean(wchar_t * s);
wchar_t GetAt(wchar_t* s, int pos);
wchar_t GetSimplifiedAt(wchar_t* s, int pos);
void MetaphAdd(metastring * s, char *new_str);
void MetaphAddChr(metastring * s, char new_str);
int isVowel(char chr);
char * Metaphone_PTBR(const wchar_t *str, const int max_length);
/* word bound char, as demanded. This is good when applied in long names */
char * Metaphone_PTBR_s(const wchar_t *str, const int max_length, const wchar_t separator);


#endif
