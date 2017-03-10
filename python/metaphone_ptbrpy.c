/************************************************************
 Copyright (C) 2017  Bruno Guberfain do Amaral <bruno.do.amaral@gmail.com>
 Released under the terms of the BSD license

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
    const wchar_t *str_param = NULL;
    char *code               = NULL;
    PyObject *result         = NULL;
    int max_length           = MAX_METAPHONE_LENGTH ;


    if (!PyArg_ParseTuple(args, "u|i", &str_param, &max_length))
        return NULL;

    code = Metaphone_PTBR(str_param, max_length);

    if (code) {
        result = Py_BuildValue("s#", code, strlen(code));

        free(code);
    } else {
        result = Py_BuildValue("s#", "", 0);
    }

    return result;
}
