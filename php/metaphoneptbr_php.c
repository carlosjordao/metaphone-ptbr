#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "metaphoneptbr_php.h"
#include "metaphone_ptbr.h"
#include "alloc.h"


/* {{{ doublemetaphone_functions[]
 * */
zend_function_entry metaphoneptbr_functions[] = {
//        PHP_FE(metaphone_ptbr, NULL)
        PHP_FE(metaphone_ptbr, arginfo_metaphone_ptbr)
        PHP_FE_END
};
/* }}} */

/* {{{ stem_module_entry
 *  */
zend_module_entry metaphoneptbr_module_entry = {
	STANDARD_MODULE_HEADER,
        "metaphoneptbr",
        metaphoneptbr_functions,
        NULL,//PHP_MINIT(metaphone_ptbr),
        NULL,//PHP_MSHUTDOWN(metaphone_ptbr),
        NULL,
        NULL,
        PHP_MINFO(metaphoneptbr_minfo),
        "1.0",
        STANDARD_MODULE_PROPERTIES
};
/* }}} */


#ifdef COMPILE_DL_METAPHONEPTBR
ZEND_GET_MODULE(metaphoneptbr)
#endif

PHP_MINFO_FUNCTION(metaphoneptbr_minfo)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "metaphone ptbr support", "enabled");
    php_info_print_table_end();
}


/* {{{ proto string metaphone_ptbr(string text)
 *    Break portuguese phrases down into their phonemes 
 */
#include <locale.h>
PHP_FUNCTION(metaphone_ptbr)
{
    char	*str 	= NULL,
	        *loc	= NULL,
            *result = NULL;
//    int 	str_len = 0,
    size_t 	str_len = 0,
        	ret	= 0,
	        mblength = 0,
	        sep_length = 0;
	wchar_t *stringUCS = NULL;
	/* this lefts wchar separator as a problem */
	char	*separator = NULL;

/*
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|s", &str, &str_len, &separator, &sep_length) == FAILURE) {
        return;
    }
*/
    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_STRING(str, str_len)
        Z_PARAM_OPTIONAL
        Z_PARAM_STRING(separator, sep_length)
    ZEND_PARSE_PARAMETERS_END();


	// Fixing locale part, important for correct character conversion
	loc = setlocale(LC_CTYPE, NULL);
	if( !loc || !(*loc) || !strcmp(loc,"C") || !strcmp(loc,"POSIX") ) {
		if( !(loc=setlocale(LC_CTYPE,"pt_BR.UTF-8")) ) {
			// if not set, will cause segmentation fault on Apache pthread 
			RETURN_STRING("");
			//return -1;
			return;
		}
	}

	// converting to wide char...
	// 1.1 discovering strlength
	mblength = mbstowcs(NULL,str,0);
	if( mblength < 1 ) {
		RETURN_STRING("");
		//return -2;
		return;
	}

	// 1.1 converting
	META_MALLOC(stringUCS,mblength+1,wchar_t);
	ret = mbstowcs(stringUCS, str, str_len);
	if( ret < 0 ) {
		META_FREE(stringUCS);
		RETURN_STRING("");
		//return -3;
		return;
	}

	stringUCS[mblength] = L'\0';

	// metaphonize it.
	if( sep_length <= 0 )
        	result = Metaphone_PTBR(stringUCS, MAX_METAPHONE_LENGTH);
	else
        	result = Metaphone_PTBR_s(stringUCS, MAX_METAPHONE_LENGTH, separator[0]);

	META_FREE(stringUCS);
    RETURN_STRING(result);
}
/* }}} */


