PHP_ARG_ENABLE(metaphoneptbr, Metaphone pt_BR,
[  --enable-metaphoneptbr        Enable Metaphone pt_BR extension module.])

if test "$PHP_METAPHONEPTBR" != "no"; then
    PHP_NEW_EXTENSION(metaphoneptbr, metaphoneptbr_php.c, $ext_shared)
    AC_DEFINE(HAVE_METAPHONEPTBR, 1, [Have Metaphone pt_BR library])
dnl    PHP_ADD_LIBPATH([..], )
dnl    PHP_ADD_LIBRARY([uuid],, UUID_SHARED_LIBADD)
dnl    PHP_ADD_INCLUDE([..])
dnl    PHP_SUBST(UUID_SHARED_LIBADD)
fi
