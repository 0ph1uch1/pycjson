#ifndef SIMDUTF_WRAPPER_H
#define SIMDUTF_WRAPPER_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
size_t count_utf8(const char *buf, size_t len);
size_t utf8_length_from_latin1(const char *buf, size_t len);
#ifdef __cplusplus
}
#endif
#endif //SIMDUTF_WRAPPER_H
