#include "metaphone_ptbrpy.h"

/* Docstrings */
static char module_docstring[] =
    "This module provides an interface for Metaphone for Brazilian Portuguese.";
static char phonetic_docstring[] =
    "Convert a string to its Brazilian Portuguese phonetic version.";

/* Module specification */
static PyMethodDef module_methods[] = {
    {"phonetic", phonetic_phonetic, METH_VARARGS, phonetic_docstring},
    {NULL, NULL, 0, NULL}
};

/* Initialize the module */
PyMODINIT_FUNC initmetaphoneptbr(void)
{
    PyObject *m = Py_InitModule3("metaphoneptbr", module_methods, module_docstring);
    if (m == NULL)
        return;
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
