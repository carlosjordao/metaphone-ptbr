#ifndef METAPHONE_ALLOC
#define METAPHONE_ALLOC

#include "php.h"


#define META_MALLOC(v, n, t)    (v = (t*)emalloc(((n)*sizeof(t))))
#define META_REALLOC(v, n, t)   (v = (t*)erealloc((v),((n)*sizeof(t))))
#define META_FREE(x)            efree((x))

#endif
