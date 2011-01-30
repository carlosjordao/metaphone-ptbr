#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	int interseccao, len1, len2, aux, aux2;
	float indice=0;

	interseccao = len1 = len2 = aux = aux2 = 0;
	
	if( argc < 3 ) {
		printf("uso: %s palavra1 palavra2\nresultado: grau de semelhança entre ambas\n",argv[0]);
		return 1;
	}

	len1 = strlen(argv[1])-1;
	len2 = strlen(argv[2])-1;

	if( len1==1 || len2==1 ) {
		printf("as palavras precisam ter tamanho > 1\n");
		return 2;
	}
	
	for( aux=0; aux<len1; aux++ ) {
		for( aux2=0; aux2<len2; aux2++ ) {
			
//			printf("%c%c == %c%c ?\n", toupper(argv[1][aux]), toupper(argv[1][aux+1]), toupper(argv[2][aux2]), toupper(argv[2][aux2+1]));
			if( toupper(argv[1][aux])   == toupper(argv[2][aux2])  &&
			    toupper(argv[1][aux+1]) == toupper(argv[2][aux2+1])
			)
				interseccao++;
		}
	}
	indice = 2.0*interseccao/(float)(len1+len2);

	printf("%f\n",indice);
	return 0;
}
