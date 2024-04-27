#include "str.h"

#include "simdutf_wrapper.h"

#include <emmintrin.h> // SSE2
#include <immintrin.h> // AVX

bool is_four_byte(const unsigned char *buf, size_t len) {
    int i;
    for (i = 0; i + 16 <= len; i += 16) {
        __m128i in = _mm_loadu_si128((const void *) (buf + i + 16));
        __m128i val = _mm_set1_epi8((unsigned char) 239);
        uint16_t mask = _mm_cmpgt_epu8_mask(in, val);
        if (mask != 0) {
            return true;
        }
    }
    for (; i < len; i++) {
        if (buf[i] > 239) return true;
    }
    return false;
}

bool check_latin1_2bytes(const unsigned char a, const unsigned char b) {
    return ((a & 0b00011111) << 6 | (b & 0b00111111)) <= 0xFF;
}

bool is_one_byte(const unsigned char *buf, size_t len) {
    // TODO check possible invalid read
    int i;
    __m128i unicode_mask1 = _mm_loadu_si128("\\u\\u\\u\\u\\u\\u\\u\\u");
    __m128i unicode_mask2 = _mm_loadu_si128("0\\u\\u\\u\\u\\u\\u\\u0");
    __m128i max_onebyte = _mm_set1_epi8((unsigned char) 0b10000000);
    for (i = 0; i + 16 <= len; i += 16) {
        __m128i in = _mm_loadu_si128((const void *) (buf + i));
        // if not all bytes are utf8 1bytes sequence in this batch
        if (_mm_cmpgt_epu8_mask(in, max_onebyte) != 0) {
            for (int j = 0; j < 16; j++) {
                if (buf[i + j] & 0b10000000) {
                    if (buf[i + j] & 0b01000000) {
                        // if it is 3 bytes utf8 seuqence
                        // OR it is 2 bytes utf8 sequence with unicode > 0xff
                        if ((buf[i + j] & 0b00100000) || (i + j + 1 < len && !check_latin1_2bytes(buf[i + j], buf[i + j + 1]))) {
                            return false;
                        }
                        j++;
                    } else {
                        // continue bytes
                        // it is already checked in last batch
                        // do nothing then
                    }
                }
            }
        }
        int result = _mm_cmpeq_epi8_mask(in, unicode_mask1);
        if (result != 0) {
            for (int ii = 0; ii < 16; ii += 2) {
                if (((result >> (16 - ii - 2)) & 0b11) == 0b11 && i + ii + 4 < len && i + ii - 1 >= 0 && buf[i + ii - 1] != '\\') {
                    if (buf[i + ii] != '0' || buf[i + ii + 1] != '0') {
                        return false;
                    }
                    // skip \u XXXX
                    ii += 4;
                }
            }
        }
        result = _mm_cmpeq_epi8_mask(in, unicode_mask2) >> 1;
        if (result != 0) {
            for (int ii = 2; ii < 16; ii += 2) {
                if (((result >> (16 - ii - 2)) & 0b11) == 0b11 && i + ii + 4 < len && i + ii - 1 - 1 >= 0 && buf[i + ii - 1 - 1] != '\\') {
                    if (buf[i + ii - 1] != '0' || buf[i + ii + 1 - 1] != '0') {
                        return false;
                    }
                    // skip \u XXXX
                    ii += 4;
                }
            }
        }
    }
    // handle remaining bytes
    for (; i < len; i++) {
        if (buf[i] & 0b10000000) {
            if (buf[i] & 0b01000000) {
                // if it is 3 bytes utf8 seuqence
                // OR it is 2 bytes utf8 sequence with unicode > 0xff
                if ((buf[i] & 0b00100000) || !check_latin1_2bytes(buf[i], buf[i + 1])) {
                    return false;
                }
                // skip the next continue byte of 2bytes sequence
                i++;
            } else {
                // it is a continue byte
                // the only possible this case happened is the last byte of last batch is a leading byte
                // and we already handle it in last byte
                // do nothing then
            }
        }
        if (buf[i] == '\\') {
            if (buf[i + 1] == 'u') {
                // skil \u
                i += 2;
                if (buf[i] != '0' || buf[i + 1] != '0') {
                    return false;
                }
                // skip XXXX
                i += 4;
            } else {
                // skip next escaped char
                i++;
            }
        }
    }
    return true;
}

#define PARSE_STRING_CHAR_MATCHER(x, ptr, t) \
    case 'b':                                \
        *ptr++ = (t) '\b';                   \
        break;                               \
    case 'f':                                \
        *ptr++ = (t) '\f';                   \
        break;                               \
    case 'n':                                \
        *ptr++ = (t) '\n';                   \
        break;                               \
    case 'r':                                \
        *ptr++ = (t) '\r';                   \
        break;                               \
    case 't':                                \
        *ptr++ = (t) '\t';                   \
        break;                               \
    case '\"':                               \
    case '\\':                               \
    case '/':                                \
        *ptr++ = (t) (x);                    \
        break;


/* parse 4 digit hexadecimal number */
static unsigned parse_hex4(const unsigned char *const input) {
    unsigned int h = 0;
    Py_ssize_t i = 0;
    for (i = 0; i < 4; i++) {
        /* parse digit */
        if ((input[i] >= '0') && (input[i] <= '9')) {
            h += (unsigned int) input[i] - '0';
        } else if ((input[i] >= 'A') && (input[i] <= 'F')) {
            h += (unsigned int) 10 + input[i] - 'A';
        } else if ((input[i] >= 'a') && (input[i] <= 'f')) {
            h += (unsigned int) 10 + input[i] - 'a';
        } else /* invalid */
        {
            return 0;
        }

        if (i < 3) {
            /* shift left to make place for the next nibble */
            h = h << 4;
        }
    }

    return h;
}

int get_utf8_type(uint32_t unciode_value) {
    if (unciode_value <= 0xFF) { // should be 0x7F
        return 1;
    } else if (unciode_value <= 0xFFFF) { // should be 0x7FF
        return 2;
    } else { // no 3 bytes utf8 in python
        return 4;
    }
}

int get_unicode_value_usc4(const char *str, Py_UCS4 *re) {
    assert(re != NULL);
    if (str[0] & 0b10000000 && !str[0] & 0b01000000) {
        // Continue bytes should be skipped.
        return 0;
    }
    if (str[0] & 0b10000000) {
        if (str[0] & 0b00100000) {
            if (str[0] & 0b00010000) {
                *re = ((str[0] & 0b111) << 18) + ((str[1] & 0b111111) << 12) + ((str[2] & 0b111111) << 6) + (str[3] & 0b111111);
                return 4;
            } else {
                *re = ((str[0] & 0b1111) << 12) + ((str[1] & 0b111111) << 6) + (str[2] & 0b111111);
                return 3;
            }
        } else {
            *re = ((str[0] & 0b11111) << 6) + (str[1] & 0b111111);
            return 2;
        }
    } else {
        *re = str[0] & 0b1111111;
        return 1;
    }
}

int get_unicode_value_usc2(const char *str, Py_UCS2 *re) {
    assert(re != NULL);
    if (str[0] & 0b10000000 && !str[0] & 0b01000000) {
        // Continue bytes should be skipped.
        *re = -1;
        return 0;
    }
    if (str[0] & 0b10000000) {
        if (str[0] & 0b00100000) {
            if (str[0] & 0b00010000) {
                // NOPE
                *re = -1;
                return 0;
            } else {
                *re = ((str[0] & 0b1111) << 12) + ((str[1] & 0b111111) << 6) + (str[2] & 0b111111);
                return 3; // 3 bytes can be represented under 0xffff
            }
        } else {
            *re = ((str[0] & 0b11111) << 6) + (str[1] & 0b111111);
            return 2;
        }
    } else {
        *re = str[0] & 0b1111111;
        return 1;
    }
}

int get_unicode_value_usc1(const char *str, Py_UCS1 *re) {
    assert(re != NULL);
    if (str[0] & 0b10000000 && !str[0] & 0b01000000) {
        // Continue bytes should be skipped.
        *re = -1;
        return 0;
    }
    if (str[0] & 0b10000000) {
        if (str[0] & 0b00100000) {
            // NOPE
            *re = -1;
            return 0;
        } else {
            *re = ((str[0] & 0b11111) << 6) + (str[1] & 0b111111);
            return 2; // some of 2 bytes can be represented under 0xff
        }
    } else {
        *re = str[0] & 0b1111111;
        return 1;
    }
}

bool str2unicode_1byte(PyObject **re, const char *str, const long alloc, const long num) {
    typedef Py_UCS1 t;
    *re = PyUnicode_New(alloc, 0xFF);
    if (*re == NULL) {
        PyErr_Format(PyExc_MemoryError, "Failed to parse string: allocation failure");
        return false;
    }
    t *data = (t *) ((PyCompactUnicodeObject *) *re + 1);
    long real_len = 0;
    for (int i = 0; i < num; i++) {
        // no overflow
        assert(real_len < alloc);
        if (str[i] == '\\') {
            switch (str[++i]) {
                PARSE_STRING_CHAR_MATCHER(str[i], data, char)
                case 'u':
                case 'U':
                    *data++ = (t) parse_hex4((unsigned char *) (str + i + 1));
                    i += 4;
                    break;
                default:
                    PyErr_Format(PyExc_ValueError, "Failed to parse string: invalid escape sequence(%d)\nposition: %d", str[i], i);
                    return false;
            }
        } else {
            const int skip = get_unicode_value_usc1(str + i, data++);
            if (skip == 0) {
                PyErr_SetString(PyExc_ValueError, "Invalid utf8 string.");
                return false;
            }
            i += skip - 1;
        }
        real_len++;
    }
    *data = 0;
    // PyUnicode_Resize(re, real_len);
    assert(real_len == alloc); // TODO remove real_len after testing
    return true;
}

bool str2unicode_2byte(PyObject **re, const char *str, const long alloc, const long num) {
    typedef Py_UCS2 t;
    *re = PyUnicode_New(alloc, 0xFFFF);
    if (*re == NULL) {
        PyErr_Format(PyExc_MemoryError, "Failed to parse string: allocation failure");
        return false;
    }
    t *data = (t *) ((PyCompactUnicodeObject *) *re + 1);
    long real_len = 0;
    for (int i = 0; i < num; i++) {
        // no overflow
        assert(real_len < alloc);
        if (str[i] == '\\') {
            switch (str[++i]) {
                PARSE_STRING_CHAR_MATCHER(str[i], data, t)
                case 'u':
                case 'U':
                    *data++ = (t) parse_hex4((unsigned char *) (str + i + 1));
                    i += 4;
                    break;
                default:
                    PyErr_Format(PyExc_ValueError, "Failed to parse string: invalid escape sequence(%d)\nposition: %d", str[i], i);
                    return false;
            }
        } else {
            const int skip = get_unicode_value_usc2(str + i, data++);
            if (skip == 0) {
                PyErr_SetString(PyExc_ValueError, "Invalid utf8 string.");
                return false;
            }
            i += skip - 1;
        }
        real_len++;
    }
    *data = 0;
    // PyUnicode_Resize(re, real_len);
    assert(real_len == alloc); // TODO remove real_len after testing
    return true;
}

bool str2unicode_4byte(PyObject **re, const char *str, const long alloc, const long num) {
    typedef Py_UCS4 t;
    *re = PyUnicode_New(alloc, 0x10ffff);
    if (*re == NULL) {
        PyErr_Format(PyExc_MemoryError, "Failed to parse string: allocation failure");
        return false;
    }
    t *data = (t *) ((PyCompactUnicodeObject *) *re + 1);
    long real_len = 0;
    for (int i = 0; i < num; i++) {
        // no overflow
        assert(real_len < alloc);
        if (str[i] == '\\') {
            switch (str[++i]) {
                PARSE_STRING_CHAR_MATCHER(str[i], data, t)
                case 'u':
                case 'U':
                    *data++ = (t) parse_hex4((unsigned char *) (str + i + 1));
                    i += 4;
                    break;
                default:
                    PyErr_Format(PyExc_ValueError, "Failed to parse string: invalid escape sequence(%d)\nposition: %d", str[i], i);
                    return false;
            }
        } else {
            const int skip = get_unicode_value_usc4(str + i, data++);
            if (skip == 0) {
                PyErr_SetString(PyExc_ValueError, "Invalid utf8 string.");
                return false;
            }
            i += skip - 1;
        }
        real_len++;
    }
    *data = 0;
    // PyUnicode_Resize(re, real_len);
    assert(real_len == alloc); // TODO remove real_len after testing
    return true;
}