#ifndef DCONV_WRAPPER_H
#define DCONV_WRAPPER_H
#include <stdbool.h>

int dconv_d2s(double value, char *buf, int buflen, int *strlength, bool allow_nan);
double dconv_s2d(const char *buffer, int length, int *processed_characters_count) ;

#endif //DCONV_WRAPPER_H
