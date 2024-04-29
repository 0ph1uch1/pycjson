#ifndef DCONV_WRAPPER_H
#define DCONV_WRAPPER_H
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
int dconv_d2s(double value, char *buf, int buflen, int *strlength, bool allow_nan);
double dconv_s2d(const char *buffer, int length, int *processed_characters_count);
#ifdef __cplusplus
}
#endif
#endif //DCONV_WRAPPER_H
