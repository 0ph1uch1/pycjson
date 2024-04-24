#ifndef STR_H
#define STR_H
#include <Python.h>
#include <stdbool.h>

bool str2unicode_1byte(PyObject **re, const char *str, long alloc, long num);
bool str2unicode_2byte(PyObject **re, const char *str, long alloc, long num);
bool str2unicode_3byte(PyObject **re, const char *str, long alloc, long num);
bool str2unicode_4byte(PyObject **re, const char *str, long alloc, long num);

#endif //STR_H
