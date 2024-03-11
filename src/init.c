#include <Python.h>
#include "version.h"

#define MODULE_STATE(o) ((modulestate *)PyModule_GetState(o))

PyObject *pycJSON_Encode(PyObject *self, PyObject *args, PyObject *kwargs);
PyObject *pycJSON_Decode(PyObject *self, PyObject *args, PyObject *kwargs);
PyObject *pycJSON_FileEncode(PyObject *self, PyObject *args, PyObject *kwargs);
PyObject *pycJSON_DecodeFile(PyObject *self, PyObject *args, PyObject *kwargs);

PyObject *JSONDecodeError = NULL;

static PyMethodDef pycJSON_Methods[] = {
    {"encode", (PyCFunction)pycJSON_Encode, METH_VARARGS | METH_KEYWORDS, "Converts arbitrary object recursively into JSON. "},
    {"decode", (PyCFunction)pycJSON_Decode, METH_VARARGS | METH_KEYWORDS, "Converts JSON as string to dict object structure."},
    {"dumps", (PyCFunction)pycJSON_Encode, METH_VARARGS | METH_KEYWORDS, "Converts arbitrary object recursively into JSON. "},
    {"loads", (PyCFunction)pycJSON_Decode, METH_VARARGS | METH_KEYWORDS, "Converts JSON as string to dict object structure."},
    {"dump", (PyCFunction)pycJSON_FileEncode, METH_VARARGS | METH_KEYWORDS, "Converts arbitrary object recursively into JSON file. "},
    {"load", (PyCFunction)pycJSON_DecodeFile, METH_VARARGS | METH_KEYWORDS, "Converts JSON as file to dict object structure."},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

static int module_traverse(PyObject *m, visitproc visit, void *arg);
static int module_clear(PyObject *m);
static void module_free(void *m);
typedef struct
{
    PyObject *type_decimal;
} modulestate;

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "cjson",
    0,                   /* m_doc */
    sizeof(modulestate), /* m_size */
    pycJSON_Methods,     /* m_methods */
    NULL,                /* m_slots */
    module_traverse,     /* m_traverse */
    module_clear,        /* m_clear */
    module_free          /* m_free */
};

static int module_traverse(PyObject *m, visitproc visit, void *arg)
{
    Py_VISIT(MODULE_STATE(m)->type_decimal);
    return 0;
}

static int module_clear(PyObject *m)
{
    Py_CLEAR(MODULE_STATE(m)->type_decimal);
    return 0;
}

static void module_free(void *m)
{
    module_clear((PyObject *)m);
}

PyMODINIT_FUNC PyInit_cjson(void)
{
    PyObject *module;

#ifndef PYPY_VERSION
    // This function is not supported in PyPy.
    if ((module = PyState_FindModule(&moduledef)) != NULL)
    {
        Py_INCREF(module);
        return module;
    }
#endif

    module = PyModule_Create(&moduledef);
    if (module == NULL)
    {
        return NULL;
    }

    PyModule_AddStringConstant(module, "__version__", PYCJSON_VERSION);

#ifndef PYPY_VERSION
    PyObject *mod_decimal = PyImport_ImportModule("decimal");
    if (mod_decimal)
    {
        PyObject *type_decimal = PyObject_GetAttrString(mod_decimal, "Decimal");
        assert(type_decimal != NULL);
        MODULE_STATE(module)->type_decimal = type_decimal;
        Py_DECREF(mod_decimal);
    }
    else
        PyErr_Clear();
#endif

    JSONDecodeError = PyErr_NewException("cjson.JSONDecodeError", PyExc_ValueError, NULL);
    Py_XINCREF(JSONDecodeError);
    if (PyModule_AddObject(module, "JSONDecodeError", JSONDecodeError) < 0)
    {
        Py_XDECREF(JSONDecodeError);
        Py_CLEAR(JSONDecodeError);
        Py_DECREF(module);
        return NULL;
    }

    return module;
}