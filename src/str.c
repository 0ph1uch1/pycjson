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

#define GET_UNICODE_VALUE                                                                                                                  \
    assert(!(str[i] & 0b10000000 && !str[i] & 0b01000000));                                                                                \
    if (str[i] & 0b10000000) {                                                                                                             \
        if (str[i] & 0b00100000) {                                                                                                         \
            if (str[i] & 0b00010000) {                                                                                                     \
                *data++ = ((str[i++] & 0b111) << 18) + ((str[i++] & 0b111111) << 12) + ((str[i++] & 0b111111) << 6) + (str[i] & 0b111111); \
            } else {                                                                                                                       \
                *data++ = ((str[i++] & 0b1111) << 12) + ((str[i++] & 0b111111) << 6) + (str[i] & 0b111111);                                \
            }                                                                                                                              \
        } else {                                                                                                                           \
            *data++ = ((str[i++] & 0b11111) << 6) + (str[i] & 0b111111);                                                                   \
        }                                                                                                                                  \
    } else {                                                                                                                               \
        *data++ = (str[i] & 0b1111111);                                                                                                    \
    }

bool str2unicode_1byte(PyObject **re, const char *str, const long alloc, const long num) {
    *re = PyUnicode_New(alloc - 1, 0xFF);
    if (*re == NULL) {
        PyErr_Format(PyExc_MemoryError, "Failed to parse string: allocation failure");
        return false;
    }
    char *data = (char *) ((PyCompactUnicodeObject *) *re + 1);
    long real_len = 0;
    for (int i = 0; i < num - 1; i++) {
        if (str[i] == '\\') {
            switch (str[++i]) {
                PARSE_STRING_CHAR_MATCHER(str[i], data, char)
                case 'u':
                case 'U':
                    *data++ = (char) parse_hex4((unsigned char *) (str + i + 1));
                    i += 4;
                    break;
                default:
                    PyErr_Format(PyExc_ValueError, "Failed to parse string: invalid escape sequence(%d)\nposition: %d", str[i], i);
                    return false;
            }
        } else
            *data++ = str[i];
        real_len++;
    }
    *data = '\x00';
    PyUnicode_Resize(re, real_len);
    return true;
}

bool str2unicode_2byte(PyObject **re, const char *str, const long alloc, const long num) {
    typedef Py_UCS2 t;
    *re = PyUnicode_New(alloc - 1, 0xFFFF);
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
            GET_UNICODE_VALUE
        }
        real_len++;
    }
    *data = '\x00';
    PyUnicode_Resize(re, real_len);
    return true;
}

bool str2unicode_3byte(PyObject **re, const char *str, const long alloc, const long num) {
    typedef Py_UCS4 t;
    *re = PyUnicode_New(alloc - 1, 0x10ffff);
    if (*re == NULL) {
        PyErr_Format(PyExc_MemoryError, "Failed to parse string: allocation failure");
        return false;
    }
    t *data = (t *) ((PyCompactUnicodeObject *) *re + 1);
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
            GET_UNICODE_VALUE
        }
    }
    *data = '\x00';
    PyUnicode_Resize(re, data - (t *) ((PyCompactUnicodeObject *) *re + 1));
    return true;
}

bool str2unicode_4byte(PyObject **re, const char *str, const long alloc, const long num) {
    typedef Py_UCS4 t;
    *re = PyUnicode_New(alloc - 1, 0x10ffff);
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
            GET_UNICODE_VALUE
        }
        real_len++;
    }
    *data = '\x00';
    PyUnicode_Resize(re, real_len);
    return true;
}