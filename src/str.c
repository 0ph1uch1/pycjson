#include "str.h"

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

int get_unicode_value(const char *str, Py_UCS4 *re) {
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

bool str2unicode_1byte(PyObject **re, const char *str, const long alloc, const long num) {
    typedef Py_UCS1 t;
    *re = PyUnicode_New(alloc, 0xFF); // TODO must be at least alloc + 1 idk why
    if (*re == NULL) {
        PyErr_Format(PyExc_MemoryError, "Failed to parse string: allocation failure");
        return false;
    }
    t *data = (t *) ((PyCompactUnicodeObject *) *re + 1);
    long real_len = 0;
    for (int i = 0; i < num - 1; i++) {
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
            const int skip = get_unicode_value(str + i, data++);
            if (skip == 0) {
                PyErr_SetString(PyExc_ValueError, "Invalid utf8 string.");
                return false;
            }
            i += skip - 1;
        }
        real_len++;
    }
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
    for (int i = 0; i < num - 1; i++) {
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
            const int skip = get_unicode_value(str + i, data++);
            if (skip == 0) {
                PyErr_SetString(PyExc_ValueError, "Invalid utf8 string.");
                return false;
            }
            i += skip - 1;
        }
        real_len++;
    }
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
    for (int i = 0; i < num - 1; i++) {
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
            const int skip = get_unicode_value(str + i, data++);
            if (skip == 0) {
                PyErr_SetString(PyExc_ValueError, "Invalid utf8 string.");
                return false;
            }
            i += skip - 1;
        }
        real_len++;
    }
    // PyUnicode_Resize(re, real_len);
    assert(real_len == alloc); // TODO remove real_len after testing
    return true;
}