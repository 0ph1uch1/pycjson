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
// check if buf is utf8 4 bytes sequence
bool is_four_byte(const unsigned char *buf, size_t len);
bool is_one_byte(const unsigned char *buf, size_t len);

#endif //STR_H
