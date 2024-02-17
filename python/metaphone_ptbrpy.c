#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "metaphone_ptbrpy.h"
#include "../source/metaphone_ptbr.h"

// http://python3porting.com/cextensions.html

/* Docstrings */
static char module_docstring[] = "This module provides an interface for Metaphone for Brazilian Portuguese.";
static char phonetic_docstring[] =
    "Convert a string to its Brazilian Portuguese phonetic version.";

/* Module specification */
static PyMethodDef module_methods[] = {
    {"phonetic", phonetic_phonetic, METH_VARARGS, phonetic_docstring},
    {NULL, NULL, 0, NULL}
};

/* Initialize the module */
#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "metaphoneptbr",     /* m_name */
    module_docstring,  /* m_doc */
    -1,                  /* m_size */
    module_methods,      /* m_methods */
    NULL,                /* m_reload */
    NULL,                /* m_traverse */
    NULL,                /* m_clear */
    NULL,                /* m_free */
};

PyMODINIT_FUNC PyInit_metaphoneptbr(void)
{
    return PyModule_Create(&moduledef);;
}

#else

PyMODINIT_FUNC initmetaphoneptbr(void)
{
    Py_InitModule3("metaphoneptbr", module_methods, module_docstring);
}

#endif

static PyObject *phonetic_phonetic(PyObject *self, PyObject *args)
{
    const char *str_param	= NULL;
    char *code              = NULL;
    PyObject *result        = NULL;
    int max_length          = MAX_METAPHONE_LENGTH ;
	wchar_t phoneme[200] = {0};
	char *loc 				= NULL;


    if (!PyArg_ParseTuple(args, "s|i", &str_param, &max_length)) {
        return NULL;
	}

	loc = setlocale(LC_CTYPE, NULL);
    if( !loc || !(*loc) || !strcmp(loc,"C") || !strcmp(loc,"POSIX") )
		if( !(loc=setlocale(LC_CTYPE,"pt_BR.UTF-8")) )
			return NULL;

	mbstowcs(phoneme, str_param, 199);

    code = Metaphone_PTBR(phoneme, max_length);

    if (code) {
        //result = Py_BuildValue("s#", code, (int)strlen(code));
        result = PyUnicode_FromString(code);
        free(code);
    } else {
        //result = Py_BuildValue("s#", "", 0);
        result = PyUnicode_FromString("");
    }

    return result;
}
