#ifndef DCONV_WRAPPER_H
#define DCONV_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif
enum FLAGS{
  // d2s/encoding flags
  NO_FLAGS = 0,
  EMIT_POSITIVE_EXPONENT_SIGN = 1,
  EMIT_TRAILING_DECIMAL_POINT = 2,
  EMIT_TRAILING_ZERO_AFTER_POINT = 4,
  UNIQUE_ZERO = 8,
  NO_TRAILING_ZERO = 16,
  EMIT_TRAILING_DECIMAL_POINT_IN_EXPONENTIAL = 32,
  EMIT_TRAILING_ZERO_AFTER_POINT_IN_EXPONENTIAL = 64,

  // s2d/decoding flags
  // NO_FLAGS = 0, // duplicated
  ALLOW_HEX = 1,
  ALLOW_OCTALS = 2,
  ALLOW_TRAILING_JUNK = 4,
  ALLOW_LEADING_SPACES = 8,
  ALLOW_TRAILING_SPACES = 16,
  ALLOW_SPACES_AFTER_SIGN = 32,
  ALLOW_CASE_INSENSITIVITY = 64,
  ALLOW_CASE_INSENSIBILITY = 64,  // Deprecated
  ALLOW_HEX_FLOATS = 128,
};

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
#ifdef __cplusplus
}
#endif
#endif //DCONV_WRAPPER_H
