#define META_MALLOC(v,n,t) (v = (t*)malloc(((n)*sizeof(t))))
#define META_REALLOC(v,n,t) (v = (t*)realloc((v),((n)*sizeof(t))))
#define META_FREE(x) free((x))
