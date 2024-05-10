#include "str.h"

#include "simdutf_wrapper.h"

#include "str-utils.h"
#include <stdint.h>
#define CHECK_NOT_LATIN1_2BYTES(a, b) (((a & 0b00011111) << 6 | (b & 0b00111111)) > 0xFF)

// https://web.archive.org/web/20151229003112/http://blogs.msdn.com/b/jeuge/archive/2005/06/08/hakmem-bit-count.aspx
// input must be uint
int BitCount(unsigned int u) {
    unsigned int uCount = u - ((u >> 1) & 033333333333) - ((u >> 2) & 011111111111);
    return ((uCount + (uCount >> 3)) & 030707070707) % 63;
}

bool count_skipped(const char *buf, size_t max_len, size_t *skipped, size_t *len) {
    const __m256i escape_mask = _mm256_set1_epi8('\\');
    const __m256i end_mask = _mm256_set1_epi8('\"');
    const __m256i u_mask = _mm256_set1_epi8('u');
    // const __m256i utf8_2_mask = _mm256_set1_epi8(0b00000011);
    // const __m256i utf8_3_mask = _mm256_set1_epi8(0b00000111);
    // const __m256i utf8_4_mask = _mm256_set1_epi8(0b00001111);
    *skipped = 0;
    int i = 0;
    int skip_next = 0;

    for (; i + 32 < max_len; i += 32) {
        __m256i batch = _mm256_loadu_si256((__m256i *) (buf + i));
        // for avx512
        // __mmask32 escape_result = _mm256_cmpeq_epi8_mask(batch, escape_mask);
        // __mmask32 end_result = _mm256_cmpeq_epi8_mask(batch, end_mask);
        // __mmask32 u_result = _mm256_cmpeq_epi8_mask(batch, u_mask);
        __mmask32 escape_result = _mm256_movemask_epi8(_mm256_cmpeq_epi8(batch, escape_mask));
        __mmask32 end_result = _mm256_movemask_epi8(_mm256_cmpeq_epi8(batch, end_mask));
        __mmask32 u_result = _mm256_movemask_epi8(_mm256_cmpeq_epi8(batch, u_mask));
        // mask out first unicode
        if (skip_next > 0) {
            const unsigned int mask = 0b11111111111111111111111111111111 ^ ((1 << skip_next) - 1);
            escape_result = escape_result & mask;
            end_result = end_result & mask;
            u_result = u_result & mask;
        }
        if (escape_result == 0 && end_result == 0) {
            skip_next = 0;
            continue;
        }
        if (end_result != 0) {
            i += skip_next;
            skip_next = 0;
            if (escape_result != 0) {
                if ((end_result ^ (escape_result << 1)) != 0) {
                    // there is real ending with some escaped sequence
                    if (u_result != 0) {
                        // some unicodes
                        while (buf[i] != '\"') {
                            if (buf[i] == '\\') {
                                *skipped += 1;
                                i++;
                                if (i > 31) {
                                    // skip the escaped char in next batch
                                    skip_next = 1;
                                }
                                if (buf[i] == 'u') {
                                    if (i + 4 >= max_len) {
                                        PyErr_SetString(PyExc_ValueError, "Invalide Utf8 sequence: invalid unicode escaped sequence");
                                        return false;
                                    }
                                    *skipped += 4;
                                    i++;
                                    if (i + 4 > 31) {
                                        skip_next = i + 4 - 32;
                                    }
                                    if (CHECK_SURROGATES_UNICODE(buf + i)) {
                                        if (i + 4 + 6 >= max_len) {
                                            PyErr_SetString(PyExc_ValueError, "Invalid Utf8 sequence: invalid unicode escaped surrogate sequence");
                                            return false;
                                        }
                                        *skipped += 6;
                                        if (i + 4 + 6 > 31) {
                                            skip_next = i + 4 + 6 - 32;
                                        }
                                        i += 6;
                                    }
                                    i += 3;
                                } else {
                                    // do nothing
                                }
                            }
                            i++;
                        }
                    } else {
                        while (buf[i] != '\"') {
                            if (buf[i] == '\\') {
                                *skipped += 1;
                                i++;
                            }
                            i++;
                        }
                    }
                    *len = i - 1 + 1;
                    return true;
                }
                // there is not ending but some escapes
                // handled by following block
            } else {
                // real ending without any escape
                while (buf[i++] != '\"') {}
                *len = i - 1 - 1 + 1;
                return true;
            }
        }
        // escape_result != 0
        // there are some escaped sequence without ending
        if (u_result != 0) {
            int j = skip_next;
            skip_next = 0;
            for (; j < 32; j++) {
                if (buf[i + j] == '\\') {
                    *skipped += 1;
                    j++;
                    if (j > 31) {
                        skip_next = 1;
                    }
                    if (buf[i + j] == 'u') {
                        if (i + j + 4 >= max_len) {
                            PyErr_SetString(PyExc_ValueError, "invalide Utf8 Sequence: invalide unicode escaped sequence");
                            return false;
                        }
                        *skipped += 4;
                        if (j + 4 > 31) {
                            skip_next = j + 4 - 32;
                        }
                        j++;
                        if (CHECK_SURROGATES_UNICODE(buf + i + j)) {
                            if (i + j + 4 + 6 >= max_len) {
                                PyErr_SetString(PyExc_ValueError, "invalid utf8 sequence");
                                return false;
                            }
                            *skipped += 6;
                            if (j + 4 + 6 > 31) {
                                skip_next = j + 4 + 6 - 32;
                            }
                            j += 6;
                        }
                        j += 3;
                    }
                }
            }
        } else {
            skip_next = 0;
            int last = 0;
            uint32_t x = escape_result;
            while (x) {
                unsigned int y = x - 1;
                if ((x >> 1) & last) {
                    x = x & y; // skipped
                } else {
                    last = x ^ y;
                    x = x & y;
                    skipped += 1;
                    if (y == 0x7fffffff) skip_next = 1;
                }
            }
        }
    }
    i += skip_next;
    for (; i < max_len; i++) {
        if (buf[i] == '"') {
            *len = i - 1 + 1;
            return true;
        }
        if (buf[i] == '\\') {
            i++;
            *skipped += 1;
            if (buf[i] == 'u') {
                if (i + 4 >= max_len) {
                    PyErr_SetString(PyExc_ValueError, "Invalid Utf8 sequence: invalid unicode escaped sequence");
                    return false;
                }
                *skipped += 4;
                i++;
                if (CHECK_SURROGATES_UNICODE(buf + i)) {
                    if (i + 4 + 6 >= max_len) {
                        PyErr_SetString(PyExc_ValueError, "Invalid Utf8 sequence: missing surrogates 2nd byte");
                        return false;
                    }
                    *skipped += 6;
                    i += 6;
                }
                i += 3;
            }
        }
    }
    PyErr_SetString(PyExc_ValueError, "Invalid json: expect ending quote");
    return false;
}

int get_utf8_kind(const unsigned char *buf, size_t len) {
    int i;
    const __m256i unicode_mask1 = _mm256_set1_epi16(0x755c); // little endian for \\u
    const __m256i unicode_mask2 = _mm256_loadu_si256((__m256i *) "0\\u\\u\\u\\u\\u\\u\\u\\u\\u\\u\\u\\u\\u\\u\\u0");
    const __m256i min_4bytes = _mm256_set1_epi8((char) 0b11101111); // -17 or 239U
    const __m256i m256_zero = _mm256_set1_epi8((char) 0);           // 0
    const __m256i max_onebyte = _mm256_set1_epi8((char) 0x80);      // -128 or 128U
    int kind = 1;
    bool skip_next = false;
    for (i = 0; i + 32 <= len; i += 32) {
        __m256i in = _mm256_loadu_si256((const void *) (buf + i));
        if (mm256_greater8u_mask(in, min_4bytes) != 0) {
            // it is 4 bytes
            return 4;
        }
        // if not all bytes are utf8 1bytes sequence in this batch
        if (mm256_greater8u_mask(in, max_onebyte)) {
            for (int j = 0; j < 32; j++) {
                if (buf[i + j] & 0b10000000) {
                    if (buf[i + j] & 0b01000000) {
                        // if it is 3 bytes utf8 seuqence
                        // OR it is 2 bytes utf8 sequence with unicode > 0xff
                        if ((buf[i + j] & 0b00100000) || (i + j + 1 < len && CHECK_NOT_LATIN1_2BYTES(buf[i + j], buf[i + j + 1]))) {
                            kind = 2;
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

        // check unicode escape \uXXXX

        // unicode starting at even position
        __mmask32 result = _mm256_movemask_epi8(_mm256_cmpeq_epi8(in, unicode_mask1));
        __mmask32 result2 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(in, unicode_mask2)) & 0b01111111111111111111111111111110;
        if (skip_next) {
            result &= 0b11111111111111111111111111111110;
            skip_next = false;
        }
        result = (result & (result >> 1)) | (result2 & (result2 >> 1));
        if (result != 0 || buf[i + 31] == '\\') {
            for (int ii = 0; ii < 32; ii++) {
                if (buf[i + ii] == '\\') {
                    ii++;
                    if (ii == 32) {
                        skip_next = true;
                    }
                    if (i + ii + 4 < len && buf[i + ii] == 'u') {
                        if (CHECK_SURROGATES_UNICODE(buf + i + ii + 1)) {
                            return 4;
                        }
                        if (buf[i + ii + 1] != '0' || buf[i + ii + 2] != '0') {
                            kind = 2;
                        }
                        ii += 4 - 1;
                    }
                }
            }
        }
    }
    if (skip_next) {
        i++;
    }
    for (; i < len; i++) {
        if (buf[i] > 239) return 4;
        if (buf[i] & 0b10000000) {
            if (buf[i] & 0b01000000) {
                // if it is 3 bytes utf8 seuqence
                // OR it is 2 bytes utf8 sequence with unicode > 0xff
                if ((buf[i] & 0b00100000) || CHECK_NOT_LATIN1_2BYTES(buf[i], buf[i + 1])) {
                    kind = 2;
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
                if (CHECK_SURROGATES_UNICODE(buf + i)) {
                    return 4;
                }
                if (buf[i] != '0' || buf[i + 1] != '0') {
                    kind = 2;
                }
                // skip XXXX
                i += 4;
            } else {
                // skip next escaped char
                i++;
            }
        }
    }
    return kind;
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
    if (num == alloc) {
        memcpy(data, str, alloc);
        return true;
    }
    long real_len = 0;
    for (int i = 0; i < num; i++) {
        // no overflow
        assert(real_len < alloc);
        if (str[i] == '\\') {
            switch (str[++i]) {
                PARSE_STRING_CHAR_MATCHER(str[i], data, char)
                case 'u':
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
                    // surrogates
                    if (CHECK_SURROGATES_UNICODE(str + i + 1)) {
                        if (i + 4 + 6 > num) {
                            PyErr_SetString(PyExc_ValueError, "Invalid utf8 string. missing surrogates 2nd byte.");
                            return false;
                        }
                        *data++ = (t) (0x10000 + (((parse_hex4((unsigned char *) (str + i + 1)) & 0x3FF) << 10) | (parse_hex4((unsigned char *) (str + i + 1 + 6)) & 0x3FF)));
                        i += 4 + 6;
                        break;
                    }
                    *data++ = (t) parse_hex4((unsigned char *) (str + i + 1));
                    i += 4;
                    break;
                    // std json don't support \U
                    // case 'U':
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
