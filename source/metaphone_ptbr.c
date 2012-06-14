/*
 * This is a port of the Metaphone based algorithm for brazilian 
 * portuguese, developed by Prefeitura Municipal de Varzea Paulista for PHP.
 *
 *
 * OBJECTIVES:
 *
 * As Metaphone, the ideia is to compute sound like strings, but
 * in this case, just for brazilian portuguese. Why? Because would be nice
 * to have generic tool for every language, but sometimes is not desirable or
 * either maintanable. Any changes could break the original metaphone code, and 
 * one way of avoiding this is introducing more "alternate" strings. Another one
 * is creating specifics metaphone functions for each language. That's my choice
 *
 * Information on using Metaphone can be found at
 *	 http://www.codeproject.com/string/dmetaphone1.asp
 * and the original article describing it can be found at
 *	 http://www.cuj.com/documents/s=8038/cuj0006philips/
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



/* turn off assertions for embedded function */
/* add check for python module */
#ifndef NDEBUG
#define NDEBUG
#endif


#include "metaphone_ptbr.h"


/*
 * remaining perl module funcs unchanged except for declaring them static
 * and reformatting to PostgreSQL indentation and to fit in 80 cols.
 *
 */

static metastring *
NewMetaString(char *init_str)
{
	metastring *s;
	char		empty_string[] = "";

	META_MALLOC(s, 1, metastring);
	assert(s != NULL);

	if (init_str == NULL)
		init_str = empty_string;
	s->length = strlen(init_str);
	/* preallocate a bit more for potential growth */
	s->bufsize = s->length + 7;

	META_MALLOC(s->str, s->bufsize, char);
	assert(s->str != NULL);

	strncpy(s->str, init_str, s->length + 1);
	s->free_string_on_destroy = 1;

	return s;
}


static void
DestroyMetaString(metastring * s)
{
	if (s == NULL)
		return;

	if (s->free_string_on_destroy && (s->str != NULL))
		META_FREE(s->str);

	META_FREE(s);
}


static void
IncreaseBuffer(metastring * s, int chars_needed)
{
	META_REALLOC(s->str, (s->bufsize + chars_needed + 10), char);
	assert(s->str != NULL);
	s->bufsize = s->bufsize + chars_needed + 10;
}

/* 
 * Hope that fixes toupper() problem with UTF-8 characters
 */
inline static wchar_t toUpper(const wchar_t c)
{
//	if( (char)c > 96 && (char)c < 123 )
//		return (wchar_t)(c-32);

	switch( c ) {

//	case L'ç': return L'Ç';

	case L'á':
	case L'à':
	case L'ã':
	case L'â':
	case L'ä':
		return L'A';

	case L'é': 
	case L'è':
	case L'ẽ':
	case L'ê':
	case L'ë':
		return L'E';

	case L'y': 
	case L'Y': 

	case L'í': 
	case L'ì':
	case L'ĩ':
	case L'î':
	case L'ï': 
		return L'I';

	case L'ó':
	case L'ò':
	case L'õ':
	case L'ô':
	case L'ö':
		return L'O';

	case L'ú':
	case L'ù':
	case L'ũ':
	case L'û':
	case L'ü':
		return L'U';

	}

	return (wchar_t)towupper((wint_t)c);
} 

/* 
 * Just let the string in uppercase mode.
 * and, as this intend to be calling as a preparation measure, let's
 * make it clean for some cases that could become a problem.
 */
static wchar_t*
MakeUpperAndClean(wchar_t* i)
{
	wchar_t *s   =(wchar_t *)NULL, 
		*aux =(wchar_t *)NULL;

	if( !i || *i == L'\0' )
		return NULL;
	
	META_MALLOC(s,wcslen(i)+1,wchar_t);
	if( !s ) 
		return (wchar_t *)NULL;
	aux = s;

	for (; *i; i++) {
		/* clean doubled chars. Not needed in portuguese */
		if( DOUBLED_CHAR(i) )
			if( *i != L'R' && *i != L'S' )
				continue;
		*aux = toUpper(*i);
		aux++;
	}
	*aux = L'\0';

	return s;
}


static wchar_t
GetAt(wchar_t* s, int pos)
{
	if ((pos < 0) || (pos >= wcslen(s)))
		return '\0';

	return ((wchar_t) *(s + pos));
}
static wchar_t
GetSimplifiedAt(wchar_t* s, int pos)
{
	if ((pos < 0) || (pos >= wcslen(s)))
		return '\0';

/* MakeUpperAndClean() took care of this.
	if ((wchar_t) *(s+pos) == L'Y')	
		return L'I';
*/
	return ((wchar_t) *(s + pos));
}



static void
MetaphAdd(metastring * s, char *new_str)
{
	int	add_length = 0;
	

	if (new_str == NULL)
		return;

	add_length = strlen(new_str);
	if ((s->length + add_length) > (s->bufsize - 1))
		IncreaseBuffer(s, add_length);

        /* just make the copy. strcat() won't work here */
	while (*new_str)
		s->str[ s->length++ ] = *new_str++;
}

/*
 * this function has been included as most of metaphone characters
 * has only 1 byte length 
 */
static void
MetaphAddChr(metastring * s, char new_str)
{
	int			add_length;

	if (new_str == '\0')
		return;

	add_length = 1;
	if ((s->length + add_length) > (s->bufsize - 1))
		IncreaseBuffer(s, add_length); 

	s->str[s->length] = new_str;
	s->length += add_length;
}

static int
isVowel(char chr)
{
	switch (chr)
	{
		/* for all effects, 'Y' counts too (antes da revisão ortográfica, claro) */
		/* no caso, 'Y' é traduzido como 'I' durante a limpeza da string por MakeUpperAndClean() */
		case 'A': case 'E': case 'I': case 'O': case 'U':
			return 1;
	}
	return 0;
}

static
char *
Metaphone_PTBR(const wchar_t *str, const int max_length)
{
	return Metaphone_PTBR_s(str, max_length, '\0');
}

static
char *
Metaphone_PTBR_s(const wchar_t *str, const int max_length, const wchar_t separator)
{
	int			length		= 0;
	wchar_t 		*original	= NULL;
	metastring 		*primary	= NULL;
	int			current 	= 0;
	int			last		= 0;
	int			count		= 0;
	char			*code 		= NULL;
	wchar_t			current_char 	= L'\0',
				last_char 	= L'\0', 
				ahead_char 	= L'\0';

	if( !str ) 
		return NULL;

	/* we need the real length and last prior to padding */
	length 	 = wcslen(str);
	last 	 = length - 1;
	primary  = NewMetaString("");

	/* let's everything be uppercase. */
//	original = MakeUpperAndClean((wchar_t *)wcsdup(str));
	original = MakeUpperAndClean((wchar_t *)str);
	if( !original )
		return NULL;

	
	/* main loop hard-limited - should be enough for very long names */
	while (( (primary->length-count) < max_length) && (current < length))
	{
		current_char = GetSimplifiedAt(original, current);

		/* skips separator */
		if (separator == current_char) {
			MetaphAddChr(primary, separator);
			count = primary->length;
		} else
		switch (current_char)
		{
			case 'A':
			case 'E':
			case 'I':
			case 'O':
			case 'U':
				/* initials vowels after any space must stay too */
				if (WORD_EDGE(last_char)) 
					MetaphAddChr(primary, current_char);
				break;

			case 'L':
				ahead_char = GetAt(original, current+1);
				/* lha, lho. Adicionado 2009-11-09. Thx Peter Krauss. Ele estava mal-colocado */
				if (ahead_char == 'H')
					MetaphAddChr(primary, '1');
				else
				/* como em Louco, aloprado, alado, lampada, etc */
				if (isVowel(ahead_char) || WORD_EDGE(last_char))
					MetaphAddChr(primary, 'L');
				/* atualmente ignora L antes de consoantes */
				break;

			case 'T':
			case 'P': 
				/* those are special cases, from foreign names or
				 * old portuguese names sintax.
				 * Besides, should behavior as the others.
				 */
				ahead_char = GetAt(original, current+1);
				if (ahead_char == 'H')
				{
					/* phone, pharmacia, teophilo */
					if (current_char == 'P')
						MetaphAddChr(primary, 'F');
					else
						MetaphAddChr(primary, 'T');
					current++;
					break;
				}

			case 'B':
			case 'D':
			case 'F':
			case 'J': 
			case 'K':
			case 'M':
			case 'V':
				MetaphAddChr(primary, current_char);
				break;
			
			/* checar consoantes com som confuso e similares */
			case 'G':
				ahead_char = GetSimplifiedAt(original, current+1);
				switch( ahead_char ) 
				{
					case 'H':
						/* H sempre complica a vida. Se não for vogal, tratar como 'G', 
						   caso contrário segue o fluxo abaixo. */
						if( !isVowel(GetSimplifiedAt(original, current+2)) ) 
							MetaphAddChr(primary,'G');
					case 'E':
					case 'I':
						MetaphAddChr(primary,'J');
						break;
						
					default:
						MetaphAddChr(primary,'G');
						break;
				}
				break;
			
			case 'R':
				ahead_char = GetSimplifiedAt(original, current+1);
				
				/* como em andar, carro, rato */
				if (WORD_EDGE(last_char) || WORD_EDGE(ahead_char))
				{
					MetaphAddChr(primary,'2');
				}
				else if (ahead_char == 'R')
				{
					MetaphAddChr(primary,'2');
					current++;
				}
				/* como em arara */
				else if (isVowel(last_char) && isVowel(ahead_char))
				{
					MetaphAddChr(primary,'R');
					current++;
						
				/* todo o resto, como em arsenico */
				} else
					MetaphAddChr(primary,'R');
				
				break;

			case 'Z':
				ahead_char = GetAt(original, current+1);
				
				/* termina com, como em algoz */
				if (WORD_EDGE(ahead_char))
					MetaphAddChr(primary,'S');
				else
					MetaphAddChr(primary,'Z');
				break;		
		
			
			case 'N':
				ahead_char = GetAt(original, current+1);
				
				/* no português, todas as palavras terminam com 'M', exceto
				 * no caso de nomes próprios, ou estrangeiros. Para todo caso,
				 * tem som de 'M'
				 */
				if (WORD_EDGE(ahead_char))
				{
					MetaphAddChr(primary,'M');
				}
				/* aranha, nhoque, manha */
				else if (ahead_char == 'H') 
				{
					MetaphAddChr(primary,'3');
					current++;
				}
				/* duplicado... */
				else if (last_char != 'N')
				{
					MetaphAddChr(primary,'N');
				}
				break;
				
			case 'S':
				ahead_char = GetSimplifiedAt(original, current+1);
				
				/* aSSar */
				if (ahead_char == 'S')
				{
					MetaphAddChr(primary,'S');
					last_char = ahead_char;
					current++;
				}
				/* mais estrangeirismo: sheila, mishel, e compatibilidade sonora com sobrenomes estrangeiros (japoneses) */
				else if (ahead_char == 'H')
				{
					MetaphAddChr(primary,'X');
					current++;
				}
				/* como em asa */
				else if (isVowel(last_char) && isVowel(ahead_char))
				{
					MetaphAddChr(primary,'Z');
				}
				/* special cases = 'SC' */
				else if (ahead_char == 'C') 
				{
					wchar_t ahead2_char = GetSimplifiedAt(original, current+2);
					switch (ahead2_char)
					{  /* aSCEnder, laSCIvia */
						case L'E': case L'I':
							MetaphAddChr(primary,'S');
							current += 2;
							break;
							
						/* maSCAvo, aSCO, auSCUltar */
						case L'A': case L'O': case L'U':
							MetaphAdd(primary,"SK");
							current += 2;
							break;

						/* estrangeirismo tal como scheila. */
						case L'H':
							MetaphAddChr(primary,'X');
							current += 2;
							break;
							
						/* mesclado */
						default:
							MetaphAddChr(primary,'S');
							current ++;
							break;
					}
				}
				else
					/* catch all - deve pegar atrás e sapato */
					MetaphAddChr(primary,'S');
				break;
			
			/* there is too many exceptions to work on... ahh! */
			case 'X':
			{
				wchar_t last2_char = GetAt(original,current-2);
				ahead_char = GetSimplifiedAt(original,current+1);

				/* fax, anticlímax e todos terminados com 'X' */
				if (WORD_EDGE(ahead_char)) 
				{
					/* o som destes casos: 
					 * MetaphAdd(primary,"KS"); 
					 * para manter compatibilidade com outra implementação, usar abaixo 
					 * como em: Felix, Alex 
					 * Na verdade, para o computador tanto faz. Se todos usarem o mesmo
					 * significado, o computador sabe q são iguais, não que som q tem.
					 * A discussão está na representação acurada ou não da fonética.
					 */
					MetaphAdd(primary,"X");
				}
				/* ...ex... */
				else if (last_char == 'E') 
				{
					if( isVowel(ahead_char) )
					{
						/* começados com EX. Exonerar, exército, executar, exemplo, exame, exílio = ex + vowel
						 * exuberar
						 */
						if (WORD_EDGE(last2_char))
						{
							/* deixado com o som original dele */
							MetaphAddChr(primary,'Z'); 
						}
						else switch(ahead_char) 
						{
						case 'E': case 'I':
							/* México, mexerica, mexer */
							MetaphAddChr(primary,'X');
							current ++;
							break;
						default:
							/* Anexar, sexo, convexo, nexo, circunflexo
							 * sexual
							 * inclusive Alex e Alexandre, o que eh
							 * bom, pois há Aleksandro ou Alex sandro
							 * OBS: texugo cai aqui.
							 */
							MetaphAdd(primary,"KS");
							current ++;
							break;
						}
					}
					/* exceção, exceto */
					else if (ahead_char == 'C')
					{
						MetaphAddChr(primary,'S');
						current++;
					/* expatriar, experimentar, extensão, exterminar. Infelizmente, êxtase cai aqui */
					} else if (ahead_char == 'P' || ahead_char == 'T' )
						MetaphAdd(primary,"S");
					/* catch all exceptions */
					else
						MetaphAdd(primary,"KS");
				}
				/* parece que certas sílabas predecessoras do 'x' como 
				 * 'ca' em 'abacaxi' provocam o som de 'CH' no 'x'.
				 * com exceção do 'm', q é mais complexo.
				 */
				else if (isVowel(last_char)) 
				{
					/* faxina. Fax é tratado acima. */
					switch (last2_char)
					{
					/* encontros vocálicos */
					case 'A': case 'E': case 'I': case 'O': case 'U': /* caixa, trouxe, abaixar, frouxo, guaxo, Teixeira */
					case 'C': /* coxa, abacaxi */
					case 'K':
					case 'G': /* gaxeta */
					case 'L': /* laxante, lixa, lixo */
					case 'R': /* roxo, bruxa */
					case 'X': /* xaxim */
						MetaphAddChr(primary,'X');
						break;

					default:
						/* táxi, axila, axioma, tóxico, fixar, fixo, monóxido, óxido */
						/* maxilar e enquadra máximo aqui tb, embora não seja correto. */
						MetaphAdd(primary,"KS");
						break;
					} 
				}
				/* anything else... enxame, enxada, -- catch all exceptions :( */
				else
					MetaphAddChr(primary,'X');
			}
				break;							

			/* ca, ce, ci, co, cu */
			case 'C':
				ahead_char = GetSimplifiedAt(original,current+1);
				switch(ahead_char) 
				{
				case 'E': case 'I': 
					MetaphAddChr(primary,'S');
					break;
					
				case 'H':
					/* christiano. */
					if( GetSimplifiedAt(original,current+2) == 'R' )
						MetaphAddChr(primary,'K');
					/* CHapéu, chuva */
					else 
						MetaphAddChr(primary,'X');
					current ++;
					break;

				/* Jacques - não fazer nada. Deixa o 'Q' cuidar disso 
				 * ou palavras com CK, mesma coisa.
				 */
				case 'Q':
				case 'K':
					break;
					
				default:
					MetaphAddChr(primary,'K');
					break;
				}

				break;
				
			/* 
			 * only considers the vowels after 'H' if only they are on
			 * the beginning of the word
			 */
			case 'H':
				if (WORD_EDGE(last_char))
				{ 
					ahead_char = GetSimplifiedAt(original,current+1);
					if (isVowel(ahead_char))
					{
						MetaphAddChr(primary,ahead_char);
						/* this will provoque some words behavior differently,
						 * which can be desirable, due differences between
						 * sounds and writting. Ex: HOSANA will be mapped to
						 * 'S' sound, instead 'Z'.
						 * OBS: para voltar à representação de Z, comente a linha abaixo
						 */ 
						current ++;
					}
				} 					
				break;
			
			case 'Q':
				MetaphAddChr(primary,'K');
				break;

			case 'W':
				ahead_char = GetSimplifiedAt(original,current+1);
				if (isVowel(ahead_char))
					MetaphAddChr(primary,'V');
				/* desconsiderar o W no final das palavras, por ter som de U,
 				 * ou ainda seguidos por consoantes, por ter som de U (Newton)

				 * soluções para www?
 				 */
				break;
		
			case L'Ç':
				MetaphAddChr(primary, 'S');
				break;

		}
		/* next char */
		current ++;
		
		last_char = current_char;
	}



	primary->str[ primary->length ] = '\0';

	META_MALLOC(code, current+1, char);
	if( !code ) 
		return NULL;
	strcpy(code, primary->str);

	META_FREE(original);
	DestroyMetaString(primary);

 	return code;
}


#ifdef PT_METAPHONE_MAIN

/* **************************************
 * TOOLS - metaphoneptbr package 
 *
 * command line for unix
 *
 */

int main(int argc, char **argv)
{
	char	*code	= NULL,
		*loc	= NULL;
	int	count	= 1, ret = 0;
	FILE	*fp	= NULL;
	wchar_t	buf[201] = L"\0";
	

//	setlocale(LC_CTYPE, "pt_BR.UTF-8");
	// Fixing locale part, important for correct character conversion
        loc = setlocale(LC_CTYPE, NULL);
        if( !loc || !(*loc) || !strcmp(loc,"C") || !strcmp(loc,"POSIX") ) 
                if( !(loc=setlocale(LC_CTYPE,"pt_BR.UTF-8")) ) 
			return -1;

	/*
	 *
	 */
	while (count < argc )
	{
                ret = mbstowcs(buf,argv[count++],200);
		code = Metaphone_PTBR_s(buf,MAX_METAPHONE_LENGTH,'.');
		printf("%s\n", code);
		free(code);
	}

/*
	fwide(stdin, 1);

	// read from stdin too, but don't wait for data  
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
	if( read(STDIN_FILENO, buf, 1) != EOF ) {
	    //ungetwc(buf[0], stdin);

	    while (fgetws(buf,200,stdin))
	    {
		code = Metaphone_PTBR_s(buf,MAX_METAPHONE_LENGTH,'.');
		printf("%s\n", code);
		free(code);
	    }
        }
*/
	return 0;
}
#endif

