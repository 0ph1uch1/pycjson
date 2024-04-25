#ifndef SIMDUTF_WRAPPER_H
#define SIMDUTF_WRAPPER_H
#include <stddef.h>

size_t count_utf8(const char *buf, size_t len);
size_t utf8_length_from_latin1(const char *buf, size_t len);
#endif //SIMDUTF_WRAPPER_H
