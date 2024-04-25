#ifndef STR_H
#define STR_H
#include <Python.h>
#include <stdbool.h>

int get_utf8_type(uint32_t unciode_value);
int get_unicode_value_usc4(const char *str, Py_UCS4 *re);
int get_unicode_value_usc2(const char *str, Py_UCS2 *re);
int get_unicode_value_usc1(const char *str, Py_UCS1 *re);
bool str2unicode_1byte(PyObject **re, const char *str, long alloc, long num);
bool str2unicode_2byte(PyObject **re, const char *str, long alloc, long num);
bool str2unicode_4byte(PyObject **re, const char *str, long alloc, long num);

#endif //STR_H
