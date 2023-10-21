#ifndef METAPHONE_PECL__H
#define METAPHONE_PECL__H



/*
extern zend_module_entry metaphoneptbr_module_entry;
#define phpext_metaphoneptbr_ptr &metaphoneptbr_module_entry
*/

#define PHP_METAPHONEPTBR_WORLD_VERSION "0.4"
#define PHP_METAPHONEPTBR_WORLD_EXTNAME "metaphoneptbr"

ZEND_BEGIN_ARG_INFO(arginfo_metaphone_ptbr, 1)
  ZEND_ARG_INFO(0, var_string)
  ZEND_ARG_INFO(0, var_sep)
ZEND_END_ARG_INFO()

PHP_MINFO_FUNCTION(metaphoneptbr_minfo);
PHP_FUNCTION(metaphone_ptbr);

extern zend_module_entry hello_module_entry;
#define phpext_hello_ptr &hello_module_entry


#endif /* METAPHONE_PECL__H */

