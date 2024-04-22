#ifndef DCONV_WRAPPER_H
#define DCONV_WRAPPER_H

void dconv_d2s_init(void **d2s,
                    int flags,
                    const char *infinity_symbol,
                    const char *nan_symbol,
                    char exponent_character,
                    int decimal_in_shortest_low,
                    int decimal_in_shortest_high,
                    int max_leading_padding_zeroes_in_precision_mode,
                    int max_trailing_padding_zeroes_in_precision_mode);

int dconv_d2s(void *d2s, double value, char *buf, int buflen, int *strlength);

void dconv_d2s_free(void **d2s);

void dconv_s2d_init(void **s2d, int flags, double empty_string_value,
                    double junk_string_value, const char *infinity_symbol,
                    const char *nan_symbol);

double dconv_s2d(void *s2d, const char *buffer, int length, int *processed_characters_count);

void dconv_s2d_free(void **s2d);

#endif //DCONV_WRAPPER_H
