#include <Python.h>
#include "metaphone_ptbrpy.h"
#include "../source/metaphone_ptbr.h"

// http://python3porting.com/cextensions.html

/* Docstrings */
//static char module_docstring[] = "This module provides an interface for Metaphone for Brazilian Portuguese.";
static char phonetic_docstring[] =
    "Convert a string to its Brazilian Portuguese phonetic version.";

/* Module specification */
static PyMethodDef module_methods[] = {
    {"phonetic", phonetic_phonetic, METH_VARARGS, phonetic_docstring},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "metaphoneptbr",     /* m_name */
    "This is a module",  /* m_doc */
    -1,                  /* m_size */
    module_methods,		 /* m_methods */
    NULL,                /* m_reload */
    NULL,                /* m_traverse */
    NULL,                /* m_clear */
    NULL,                /* m_free */
};

/* Initialize the module */
PyMODINIT_FUNC PyInit_metaphoneptbr(void)
{
	//PyObject *m = Py_InitModule3("metaphoneptbr", module_methods, module_docstring);
	PyObject *m = PyModule_Create(&moduledef);
	if (m == NULL)
		return NULL;
	return m;
}

static PyObject *phonetic_phonetic(PyObject *self, PyObject *args)
{
    const wchar_t *str_param = NULL;
    char *code               = NULL;
    PyObject *result         = NULL;


    if (!PyArg_ParseTuple(args, "u", &str_param))
        return NULL;

    code = Metaphone_PTBR(str_param, MAX_METAPHONE_LENGTH);

    if (code) {
        result = Py_BuildValue("s#", code, strlen(code));

        free(code);
    } else {
        result = Py_BuildValue("s#", "", 0);
    }

    return result;
}

