/*
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


***********************************************************************/



/* turn off assertions for embedded function */
/* add check for python module */
#ifndef NDEBUG
#define NDEBUG
#endif


#include "metaphone_ptbr.h"
#include "alloc.h"


 metastring *
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

	//strncpy(s->str, init_str, s->length + 1);
	memcpy(s->str, init_str, s->length + 1);
	s->free_string_on_destroy = 1;

	return s;
}


 void
DestroyMetaString(metastring * s)
{
	if (s == NULL)
		return;

	if (s->free_string_on_destroy && (s->str != NULL))
		META_FREE(s->str);

	META_FREE(s);
}


 void
IncreaseBuffer(metastring * s, int chars_needed)
{
	META_REALLOC(s->str, (s->bufsize + chars_needed + 10), char);
	assert(s->str != NULL);
	s->bufsize = s->bufsize + chars_needed + 10;
}

/*
 * Hope that fixes toupper() problem with UTF-8 characters
 */
inline static wchar_t toUpper(const wchar_t d)
{
	wchar_t c;

	c = (wchar_t)towupper((wint_t)d);

	switch( c ) {

	case L'Á':
	case L'À':
	case L'Ã':
	case L'Â':
	case L'Ä':
		return L'A';

	case L'É':
	case L'È':
	case L'Ẽ':
	case L'Ê':
	case L'Ë':
		return L'E';

	case L'Y':

	case L'Í':
	case L'Ì':
	case L'Ĩ':
	case L'Î':
	case L'Ï':
		return L'I';

	case L'Ó':
	case L'Ò':
	case L'Õ':
	case L'Ô':
	case L'Ö':
		return L'O';

	case L'Ú':
	case L'Ù':
	case L'Ũ':
	case L'Û':
	case L'Ü':
		return L'U';
	}

	return c;
}

/*
 * Just let the string in uppercase mode.
 * and, as this intend to be calling as a preparation measure, let's
 * make it clean for some cases that could become a problem.
 */
 wchar_t*
MakeUpperAndClean(wchar_t* i)
{
	wchar_t *s   =(wchar_t *)NULL,
		*aux =(wchar_t *)NULL;

        if( !i || *i == L'\0' )
		return NULL;

	/* transforma todos em maiúsculas, com algumas simplificações */
	aux = i;
	while( *aux ) {
		*aux = toUpper(*aux);
		aux++;
	}

	/* copia para o novo buffer, eliminando os duplicados. */
	META_MALLOC(s,wcslen(i)+1,wchar_t);
	if( !s )
		return (wchar_t *)NULL;

	aux = s;
	*aux = *i;
	aux++;
	i++;
	for (; *i; i++) {

		/* clean doubled chars. Not needed in portuguese, except 'R' and 'S' */
		if( DOUBLED_CHAR(i) ) {
			if( *i != L'R' && *i != L'S' )
				continue;

			/* caso mais de 2 caracteres seguidos, vá até o último. */
			while( *i && *i == *(i+1) )
				i++;
		}
		*aux = *i;
		aux++;
	}

	*aux = L'\0';
	return s;
}


 wchar_t
GetAt(wchar_t* s, int pos)
{
	if ((pos < 0) || (pos >= wcslen(s)))
		return '\0';

	return ((wchar_t) *(s + pos));
}
 wchar_t
GetSimplifiedAt(wchar_t* s, int pos)
{
	if ((pos < 0) || (pos >= wcslen(s)))
		return '\0';

	return ((wchar_t) *(s + pos));
}



 void
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
 void
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

int
isVowel(char chr)
{
	switch (chr)
	{
		/* 'Y' é traduzido como 'I' durante a limpeza da string por toUpper() */
		case 'A': case 'E': case 'I': case 'O': case 'U':
			return 1;
	}
	return 0;
}


char *
Metaphone_PTBR(const wchar_t *str, const int max_length)
{
	return Metaphone_PTBR_s(str, max_length, '\0');
}

//

char *
Metaphone_PTBR_s(const wchar_t *str, const int max_length, const wchar_t separator)
{
	int			length		= 0;
	wchar_t 		*original	= NULL,
				*tmp		= NULL;
	metastring 		*primary	= NULL;
	int			current 	= 0;
	int			count		= 0;
	char			*code 		= NULL;
	wchar_t			current_char 	= L'\0',
				last_char 	= L'\0',
				ahead_char 	= L'\0';

	if( !str )
		return NULL;

	/* we need the real length and last prior to padding */
	length 	 = wcslen(str);
	primary  = NewMetaString("");

	/* let's everything be uppercase. */
	tmp = wcsdup(str);
	original = MakeUpperAndClean(tmp);
	free(tmp);
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
							 * OBS: texugo cai aqui. Vítima de guerra.
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
                else if (ahead_char == 'L' || ahead_char == 'R' ) /* sugestão de luisfurquim@gmail.com p/ Wladimir e Wrana */
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

	//META_MALLOC(code, current+1, char);
	META_MALLOC(code, primary->length+1, char);
	if( !code )
		return NULL;
	//bzero(code, current+1);
	//memcpy(code, primary->str, current);
	memcpy(code, primary->str, primary->length);
	code[primary->length] = '\0';

	META_FREE(original);
	DestroyMetaString(primary);

 	return code;
}

