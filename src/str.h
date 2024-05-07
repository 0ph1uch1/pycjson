#ifndef STR_H
#define STR_H
#include "pycJSON.h"
#include <stdbool.h>

#define CHECK_SURROGATES_UNICODE(buf)        \
    (((buf)[0] == 'd' || (buf)[0] == 'D') && \
     ((buf)[1] == '8' || (buf)[1] == '9' || (buf)[1] == 'a' || (buf)[1] == 'b' || (buf)[1] == 'A' || (buf)[1] == 'B'))
// #define CHECK_SURROGATES_LOW_UNICODE(buf) \
//   ((buf)[0] == 'd' || (buf)[0] == 'D') &&  ((buf)[1] == 'c' || (buf)[1] == 'C')

int get_utf8_type(uint32_t unciode_value);
int get_unicode_value_usc4(const char *str, Py_UCS4 *re);
int get_unicode_value_usc2(const char *str, Py_UCS2 *re);
int get_unicode_value_usc1(const char *str, Py_UCS1 *re);
bool str2unicode_1byte(PyObject **re, const char *str, long alloc, long num);
bool str2unicode_2byte(PyObject **re, const char *str, long alloc, long num);
bool str2unicode_4byte(PyObject **re, const char *str, long alloc, long num);
// can be 1,2, or 4
int get_utf8_kind(const unsigned char *buf, size_t len);
bool count_skipped(const char *buf, size_t max_len, size_t *skipped, size_t *len);

#endif //STR_H
