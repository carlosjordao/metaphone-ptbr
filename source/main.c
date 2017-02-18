/***************************** COPYRIGHT NOTICES ***********************
  Copyright 2017, Carlos Costa Jordao <carlosjordao@gmail.com>.
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


#include "metaphone_ptbr.h"
#include "alloc.h"



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
		if( strlen(argv[count]) == 0 ) {
			count++;
			continue;
		}
                ret = mbstowcs(buf,argv[count++],200);
		code = Metaphone_PTBR_s(buf,MAX_METAPHONE_LENGTH,'.');
		if( code )
			printf("%s\n", code);
		else
			puts("error - null string");
		free(code);
	}

/* pipeline not working
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
