#include "pycJSON.h"
#define PY_SSIZE_T_CLEAN
#include "dconv_wrapper.h"
#include <Python.h>
#include <math.h>
#include <stdbool.h>
/* check if the given size is left to read in a given parse buffer (starting with 1) */
#define can_read(buffer, size) ((buffer != NULL) && (((buffer)->offset + size) <= (buffer)->length))
/* check if the buffer can be accessed at the given index (starting with 0) */
#define can_access_at_index(buffer, index) ((buffer != NULL) && (((buffer)->offset + index) < (buffer)->length))
#define cannot_access_at_index(buffer, index) (!can_access_at_index(buffer, index))
/* get a pointer to the buffer at the position */
#define buffer_at_offset(buffer) ((buffer)->content + (buffer)->offset)
#define STACK_BUFFER_SIZE 512

typedef struct internal_hooks {
    void *(CJSON_CDECL *allocate)(size_t size);
    void(CJSON_CDECL *deallocate)(void *pointer);
} internal_hooks;

static internal_hooks global_hooks = {PyMem_Malloc, PyMem_Free};
static void *dconv_s2d_ptr = NULL;

typedef struct
{
    const unsigned char *content;
    Py_ssize_t length;
    Py_ssize_t offset;
    Py_ssize_t depth; /* How deeply nested (in arrays/objects) is the input at the current offset. */
    internal_hooks hooks;
    /* kwargs */
    PyObject *object_hook;
} parse_buffer;

static bool parse_value(PyObject **item, parse_buffer *const input_buffer);

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

/* converts a UTF-16 literal to UTF-8
 * A literal can be one or two sequences of the form \uXXXX */
static unsigned char utf16_literal_to_utf8(const unsigned char *const input_pointer, const unsigned char *const input_end, unsigned char **output_pointer) {
    assert(output_pointer);
    long unsigned int codepoint = 0;
    unsigned int first_code = 0;
    const unsigned char *first_sequence = input_pointer;
    unsigned char utf8_length = 0;
    unsigned char utf8_position = 0;
    unsigned char sequence_length = 0;
    unsigned char first_byte_mark = 0;

    if ((input_end - first_sequence) < 6) {
        /* input ends unexpectedly */
        goto fail;
    }

    /* get the first utf16 sequence */
    first_code = parse_hex4(first_sequence + 2);

    /* check that the code is valid */
    if (((first_code >= 0xDC00) && (first_code <= 0xDFFF))) {
        goto fail;
    }

    /* UTF16 surrogate pair */
    if ((first_code >= 0xD800) && (first_code <= 0xDBFF)) {
        const unsigned char *second_sequence = first_sequence + 6;
        unsigned int second_code = 0;
        sequence_length = 12; /* \uXXXX\uXXXX */

        if ((input_end - second_sequence) < 6) {
            /* input ends unexpectedly */
            goto fail;
        }

        if ((second_sequence[0] != '\\') || (second_sequence[1] != 'u')) {
            /* missing second half of the surrogate pair */
            goto fail;
        }

        /* get the second utf16 sequence */
        second_code = parse_hex4(second_sequence + 2);
        /* check that the code is valid */
        if ((second_code < 0xDC00) || (second_code > 0xDFFF)) {
            /* invalid second half of the surrogate pair */
            goto fail;
        }


        /* calculate the unicode codepoint from the surrogate pair */
        codepoint = 0x10000 + (((first_code & 0x3FF) << 10) | (second_code & 0x3FF));
    } else {
        sequence_length = 6; /* \uXXXX */
        codepoint = first_code;
    }

    /* encode as UTF-8
     * takes at maximum 4 bytes to encode:
     * 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
    if (codepoint < 0x80) {
        /* normal ascii, encoding 0xxxxxxx */
        utf8_length = 1;
    } else if (codepoint < 0x800) {
        /* two bytes, encoding 110xxxxx 10xxxxxx */
        utf8_length = 2;
        first_byte_mark = 0xC0; /* 11000000 */
    } else if (codepoint < 0x10000) {
        /* three bytes, encoding 1110xxxx 10xxxxxx 10xxxxxx */
        utf8_length = 3;
        first_byte_mark = 0xE0; /* 11100000 */
    } else if (codepoint <= 0x10FFFF) {
        /* four bytes, encoding 1110xxxx 10xxxxxx 10xxxxxx 10xxxxxx */
        utf8_length = 4;
        first_byte_mark = 0xF0; /* 11110000 */
    } else {
        /* invalid unicode codepoint */
        goto fail;
    }

    /* encode as utf8 */
    for (utf8_position = (unsigned char) (utf8_length - 1); utf8_position > 0; utf8_position--) {
        /* 10xxxxxx */
        (*output_pointer)[utf8_position] = (unsigned char) ((codepoint | 0x80) & 0xBF);
        codepoint >>= 6;
    }
    /* encode first byte */
    if (utf8_length > 1) {
        (*output_pointer)[0] = (unsigned char) ((codepoint | first_byte_mark) & 0xFF);
    } else {
        (*output_pointer)[0] = (unsigned char) (codepoint & 0x7F);
    }

    *output_pointer += utf8_length;

    return sequence_length;

fail:
    return 0;
}

/* Utility to jump whitespace and cr/lf */
static parse_buffer *buffer_skip_whitespace(parse_buffer *const buffer) {
    if ((buffer == NULL) || (buffer->content == NULL)) {
        return NULL;
    }

    if (cannot_access_at_index(buffer, 0)) {
        return buffer;
    }

    while (can_access_at_index(buffer, 0) && (buffer_at_offset(buffer)[0] <= 32)) {
        buffer->offset++;
    }

    if (buffer->offset == buffer->length) {
        buffer->offset--;
    }

    return buffer;
}

/* skip the UTF-8 BOM (byte order mark) if it is at the beginning of a buffer */
static parse_buffer *skip_utf8_bom(parse_buffer *const buffer) {
    if ((buffer == NULL) || (buffer->content == NULL) || (buffer->offset != 0)) {
        return NULL;
    }

    if (can_access_at_index(buffer, 4) && (strncmp((const char *) buffer_at_offset(buffer), "\xEF\xBB\xBF", 3) == 0)) {
        buffer->offset += 3;
    }

    return buffer;
}

// define two helper macros for the string parsing
#define PARSE_STRING_CHAR_MATCHER(x) \
    case 'b':                        \
        *buffer_writer++ = '\b';     \
        break;                       \
    case 'f':                        \
        *buffer_writer++ = '\f';     \
        break;                       \
    case 'n':                        \
        *buffer_writer++ = '\n';     \
        break;                       \
    case 'r':                        \
        *buffer_writer++ = '\r';     \
        break;                       \
    case 't':                        \
        *buffer_writer++ = '\t';     \
        break;                       \
    case '\"':                       \
    case '\\':                       \
    case '/':                        \
        *buffer_writer++ = (x);      \
        break;

#define PARSE_STRING_FINALIZE                                            \
    if (buffer_ptr != NULL && buffer_ptr != parse_string_stack_buffer) { \
        input_buffer->hooks.deallocate(buffer_ptr);                      \
    }

/* Parse the input text into an unescaped cinput, and populate item. */
static bool parse_string(PyObject **item, parse_buffer *const input_buffer) {
    assert(item);
    const unsigned char *input_pointer = buffer_at_offset(input_buffer) + 1;
    const unsigned char *input_end = buffer_at_offset(input_buffer) + 1;

    unsigned char *buffer_writer = NULL;
    unsigned char *buffer_ptr = NULL;
    unsigned char parse_string_stack_buffer[STACK_BUFFER_SIZE];


    /* not a string */
    if (buffer_at_offset(input_buffer)[0] != '\"') {
        PyErr_Format(PyExc_ValueError, "Failed to parse string: it is not a string\nposition: %d", input_buffer->offset);
        goto fail;
    }

    {
        bool is_ascii = true;
        /* calculate approximate size of the output (overestimate) */
        Py_ssize_t allocation_length = 0;
        Py_ssize_t skipped_bytes = 0;
        while (((Py_ssize_t) (input_end - input_buffer->content) < input_buffer->length) && (*input_end != '\"')) {
            if (is_ascii && (*input_end & 0x80)) {
                is_ascii = false;
            }
            /* is escape sequence */
            if (input_end[0] == '\\') {
                if ((Py_ssize_t) (input_end + 1 - input_buffer->content) >= input_buffer->length) {
                    /* prevent buffer overflow when last input character is a backslash */
                    PyErr_Format(PyExc_ValueError, "Failed to parse string: buffer overflow\nposition: %d", input_buffer->offset);
                    goto fail;
                }
                if (is_ascii && input_end[1] == 'u') {
                    is_ascii = false;
                }
                skipped_bytes++;
                input_end++;
            }
            input_end++;
        }
        if (((Py_ssize_t) (input_end - input_buffer->content) >= input_buffer->length) || (*input_end != '\"')) {
            PyErr_Format(PyExc_ValueError, "Failed to parse string: string ended unexpectedly\nposition: %d", input_buffer->offset);
            goto fail; /* string ended unexpectedly */
        }

        /* This is at most how much we need for the output */
        allocation_length = (Py_ssize_t) (input_end - buffer_at_offset(input_buffer)) - skipped_bytes;

        // fast path for pure ASCII
        if (is_ascii) {
            *item = PyUnicode_New(allocation_length - 1, 127);
            if (*item == NULL) {
                PyErr_Format(PyExc_MemoryError, "Failed to parse string: allocation failure\nposition: %d", input_buffer->offset);
                goto fail; /* allocation failure */
            }
            buffer_writer = (unsigned char *) PyUnicode_1BYTE_DATA(*item);
            input_buffer->offset++;
            while (input_buffer->offset < input_buffer->length && buffer_at_offset(input_buffer)[0] != '\"') {
                if (buffer_at_offset(input_buffer)[0] != '\\') {
                    *buffer_writer++ = buffer_at_offset(input_buffer)[0];
                    input_buffer->offset++;
                }
                /* escape sequence */
                else {
                    unsigned char this_char = buffer_at_offset(input_buffer)[1];
                    switch (this_char) {
                        PARSE_STRING_CHAR_MATCHER(this_char);
                        default:
                            PyErr_Format(PyExc_ValueError, "Failed to parse string: invalid escape sequence(%d)\nposition: %d", this_char, input_buffer->offset);
                            goto fail;
                    }
                    input_buffer->offset += 2;
                }
            }
            goto success;
        }

        if (allocation_length < STACK_BUFFER_SIZE) {
            buffer_ptr = parse_string_stack_buffer;
        } else {
            unsigned char *malloc_ptr;
            malloc_ptr = (unsigned char *) input_buffer->hooks.allocate(allocation_length + sizeof(""));
            if (malloc_ptr == NULL) {
                PyErr_Format(PyExc_MemoryError, "Failed to parse string: allocation failure\nposition: %d", input_buffer->offset);
                goto fail; /* allocation failure */
            }
            buffer_ptr = malloc_ptr;
        }
    }

    buffer_writer = buffer_ptr;
    /* loop through the string literal */
    while (input_pointer < input_end) {
        if (*input_pointer != '\\') {
            *buffer_writer++ = *input_pointer++;
        }
        /* escape sequence */
        else {
            unsigned char sequence_length = 2;
            if ((input_end - input_pointer) < 1) {
                PyErr_Format(PyExc_ValueError, "Failed to parse string: buffer overflow\nposition: %d", input_buffer->offset);
                goto fail;
            }
            unsigned char this_char = input_pointer[1];
            switch (this_char) {
                PARSE_STRING_CHAR_MATCHER(this_char);
                /* UTF-16 literal */
                case 'u':
                    sequence_length = utf16_literal_to_utf8(input_pointer, input_end, &buffer_writer);
                    if (sequence_length == 0) {
                        /* failed to convert UTF16-literal to UTF-8 */
                        PyErr_Format(PyExc_ValueError, "Failed to parse string: invalid UTF-16\nposition: %d", input_buffer->offset);
                        goto fail;
                    }
                    break;

                default:
                    PyErr_Format(PyExc_ValueError, "Failed to parse string: invalid escape sequence(%d)\nposition: %d", input_pointer[1], input_buffer->offset);
                    goto fail;
            }
            input_pointer += sequence_length;
        }
    }

    /* zero terminate the output */
    *buffer_writer = '\0';
    *item = PyUnicode_FromString((char *) buffer_ptr);

    input_buffer->offset = (Py_ssize_t) (input_end - input_buffer->content);

success:
    PARSE_STRING_FINALIZE;

    input_buffer->offset++;

    return true;

fail:
    PARSE_STRING_FINALIZE;

    if (input_pointer != NULL) {
        input_buffer->offset = (Py_ssize_t) (input_pointer - input_buffer->content);
    }

    return false;
}
#undef PARSE_STRING_FINALIZE
#undef PARSE_STRING_CHAR_MATCHER

/* Build an array from input text. */
static bool parse_array(PyObject **item, parse_buffer *const input_buffer) {
    assert(item);
    if (input_buffer->depth >= CJSON_NESTING_LIMIT) {
        PyErr_Format(PyExc_ValueError, "Failed to parse array: too deeply nested\nposition: %d", input_buffer->offset);
        return false; /* to deeply nested */
    }
    input_buffer->depth++;

    if (buffer_at_offset(input_buffer)[0] != '[') {
        /* not an array */
        PyErr_Format(PyExc_ValueError, "Failed to parse array: it is not an array\nposition: %d", input_buffer->offset);
        goto fail;
    }

    input_buffer->offset++;
    buffer_skip_whitespace(input_buffer);
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == ']')) {
        /* empty array */
        *item = PyList_New(0);
        goto success;
    }

    /* check if we skipped to the end of the buffer */
    if (cannot_access_at_index(input_buffer, 0)) {
        input_buffer->offset--;
        PyErr_Format(PyExc_ValueError, "Failed to parse array: buffer overflow\nposition: %d", input_buffer->offset);
        goto fail;
    }
    /* step back to character in front of the first element */
    input_buffer->offset--;
    *item = PyList_New(0);
    /* loop through the comma separated array elements */
    do {
        /* parse next value */
        input_buffer->offset++;
        buffer_skip_whitespace(input_buffer);
        PyObject *buffer = NULL;
        if (!parse_value(&buffer, input_buffer)) {
            goto fail; /* failed to parse value */
        }
        buffer_skip_whitespace(input_buffer);
        PyList_Append(*item, buffer);
        Py_DECREF(buffer);
    } while (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == ','));
    if (cannot_access_at_index(input_buffer, 0) || buffer_at_offset(input_buffer)[0] != ']') {
        PyErr_Format(PyExc_ValueError, "Failed to parse array: expected end of array\nposition: %d", input_buffer->offset);
        goto fail; /* expected end of array */
    }

success:
    input_buffer->depth--;

    input_buffer->offset++;

    return true;

fail:
    Py_XDECREF(*item);
    *item = NULL;

    return false;
}

/* Parse the input text to generate a number, and populate the result into item. */
static bool parse_number(PyObject **item, parse_buffer *const input_buffer) {
    assert(item);
    unsigned char *after_end = NULL;
    unsigned char old_ending = '\x0';
    Py_ssize_t i = 0;
    bool dec = false;
    const unsigned char *starting_point = buffer_at_offset(input_buffer);

    if ((input_buffer == NULL) || (input_buffer->content == NULL)) {
        PyErr_Format(PyExc_ValueError, "Failed to parse number: no input\nposition: %d", input_buffer->offset);
        return false;
    }

    /* copy the number into a temporary buffer and replace '.' with the decimal point
     * of the current locale (for strtod)
     * This also takes care of '\0' not necessarily being available for marking the end of the input */
    for (i = 0; can_access_at_index(input_buffer, i); i++) {
        switch (buffer_at_offset(input_buffer)[i]) {
            case '.':
            case 'E':
            case 'e':
                dec = true;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '+':
            case '-':
                break;

            default:
                goto loop_end;
        }
    }
loop_end:
    // there is already a null terminator at the end of the buffer
    if (input_buffer->offset + i != input_buffer->length) {
        if (!can_access_at_index(input_buffer, i) || i == 0) {
            PyErr_Format(PyExc_ValueError, "Failed to parse number: expected character after number\nposition: %d", input_buffer->offset);
            return false;
        }
        old_ending = buffer_at_offset(input_buffer)[i];
        *((char *) (buffer_at_offset(input_buffer) + i)) = '\0';
    }

    if (dec) {
        // const double temp = PyOS_string_to_double((const char *) starting_point, (char **) &after_end, PyExc_OverflowError);
        // if (PyErr_Occurred()) return false;
        if (dconv_s2d_ptr == NULL) {
            dconv_s2d_init(&dconv_s2d_ptr, NO_FLAGS, 0.0, Py_NAN, "Infinity", "NaN");
        }
        int processed_characters_count = 0;
        const double temp = dconv_s2d(dconv_s2d_ptr, (const char *) starting_point, i, &processed_characters_count);
        if (i != processed_characters_count) {
            PyErr_Format(PyExc_ValueError, "Failed to parse number: invalid number, only can parse (%d/%d)\nposition: %d", processed_characters_count, i, input_buffer->offset);
            goto fail;
        }
        after_end = (unsigned char *) (starting_point + processed_characters_count);
        *item = PyFloat_FromDouble(temp);
    } else
        *item = PyLong_FromString((const char *) starting_point, (char **) &after_end, 10);
    if (starting_point == after_end || NULL == *item) {
        PyErr_Format(PyExc_ValueError, "Failed to parse number: invalid number\nposition: %d", input_buffer->offset);
        goto fail;
    }

    // restore the old ending, if it was overwritten, otherwise it will set null terminator again
    *((char *) (buffer_at_offset(input_buffer) + i)) = old_ending;
    input_buffer->offset += (Py_ssize_t) (after_end - starting_point);
    return true;

fail:
    *((char *) (buffer_at_offset(input_buffer) + i)) = old_ending;
    return false;
}

/* Build an object from the text. */
static bool parse_object(PyObject **item, parse_buffer *const input_buffer) {
    assert(item);
    if (input_buffer->depth >= CJSON_NESTING_LIMIT) {
        PyErr_Format(PyExc_ValueError, "Failed to parse dictionary: too deeply nested\nposition: %d", input_buffer->offset);
        return false; /* to deeply nested */
    }
    input_buffer->depth++;

    if (cannot_access_at_index(input_buffer, 0) || (buffer_at_offset(input_buffer)[0] != '{')) {
        PyErr_Format(PyExc_ValueError, "Failed to parse dictionary: it is not an object\nposition: %d", input_buffer->offset);
        goto fail; /* not an object */
    }

    input_buffer->offset++;
    buffer_skip_whitespace(input_buffer);
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '}')) {
        /* empty object */
        *item = PyDict_New();
        goto success;
    }

    /* check if we skipped to the end of the buffer */
    if (cannot_access_at_index(input_buffer, 0)) {
        input_buffer->offset--;
        PyErr_Format(PyExc_ValueError, "Failed to parse dictionary: buffer overflow\nposition: %d", input_buffer->offset);
        goto fail;
    }
    *item = PyDict_New();
    /* step back to character in front of the first element */
    input_buffer->offset--;
    /* loop through the comma separated array elements */
    do {
        /* parse the name of the child */
        input_buffer->offset++;
        buffer_skip_whitespace(input_buffer);
        PyObject *keyBuffer = NULL;
        if (!parse_string(&keyBuffer, input_buffer)) {
            goto fail; /* failed to parse name */
        }
        buffer_skip_whitespace(input_buffer);

        if (cannot_access_at_index(input_buffer, 0) || (buffer_at_offset(input_buffer)[0] != ':')) {
            PyErr_Format(PyExc_ValueError, "Failed to parse dictionary: expected colon\nposition: %d", input_buffer->offset);
            goto fail; /* invalid object */
        }

        /* parse the value */
        input_buffer->offset++;
        buffer_skip_whitespace(input_buffer);
        PyObject *valueBuffer = NULL;
        if (!parse_value(&valueBuffer, input_buffer)) {
            goto fail; /* failed to parse value */
        }
        buffer_skip_whitespace(input_buffer);
        PyDict_SetItem(*item, keyBuffer, valueBuffer);
        Py_DECREF(valueBuffer);
        Py_DECREF(keyBuffer);
    } while (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == ','));

    if (cannot_access_at_index(input_buffer, 0) || (buffer_at_offset(input_buffer)[0] != '}')) {
        PyErr_Format(PyExc_ValueError, "Failed to parse dictionary: expected end of object\nposition: %d", input_buffer->offset);
        goto fail; /* expected end of object */
    }

success:
    input_buffer->depth--;

    input_buffer->offset++;

    if (input_buffer->object_hook) {
        PyObject *re = PyObject_CallFunctionObjArgs(input_buffer->object_hook, *item, NULL);
        if (re == NULL) {
            PyErr_Format(PyExc_ValueError, "Failed to parse dictionary: object_hook failed\nposition: %d", input_buffer->offset);
            goto fail;
        }
        *item = re;
    }
    return true;

fail:
    Py_XDECREF(*item);
    *item = NULL;

    return false;
}

/* Parser core - when encountering text, process appropriately. */
static bool parse_value(PyObject **item, parse_buffer *const input_buffer) {
    assert(item);
    if ((input_buffer == NULL) || (input_buffer->content == NULL)) {
        PyErr_Format(PyExc_ValueError, "Failed to parse value: no input\nposition: %d", input_buffer->offset);
        return false; /* no input */
    }

    /* parse the different types of values */
    /* string */
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '\"')) {
        return parse_string(item, input_buffer);
    }
    /* number */
    if (can_access_at_index(input_buffer, 0) && ((buffer_at_offset(input_buffer)[0] == '-') || ((buffer_at_offset(input_buffer)[0] >= '0') && (buffer_at_offset(input_buffer)[0] <= '9')))) {
        /* -Infinity */
        if (can_read(input_buffer, 9) && (strncmp((const char *) buffer_at_offset(input_buffer), "-Infinity", 9) == 0)) {
            *item = PyFloat_FromDouble(-INFINITY);
            input_buffer->offset += 9;
            return true;
        }
        return parse_number(item, input_buffer);
    }
    /* array */
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '[')) {
        return parse_array(item, input_buffer);
    }
    /* object */
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '{')) {
        return parse_object(item, input_buffer);
    }
    /* null */
    if (can_read(input_buffer, 4) && (strncmp((const char *) buffer_at_offset(input_buffer), "null", 4) == 0)) {
        Py_INCREF(Py_None);
        *item = Py_None;
        input_buffer->offset += 4;
        return true;
    }
    /* false */
    if (can_read(input_buffer, 5) && (strncmp((const char *) buffer_at_offset(input_buffer), "false", 5) == 0)) {
        Py_INCREF(Py_False);
        *item = Py_False;
        input_buffer->offset += 5;
        return true;
    }
    /* true */
    if (can_read(input_buffer, 4) && (strncmp((const char *) buffer_at_offset(input_buffer), "true", 4) == 0)) {
        Py_INCREF(Py_True);
        *item = Py_True;
        input_buffer->offset += 4;
        return true;
    }
    /* Infinity */
    if (can_read(input_buffer, 8) && (strncmp((const char *) buffer_at_offset(input_buffer), "Infinity", 8) == 0)) {
        *item = PyFloat_FromDouble(INFINITY);
        input_buffer->offset += 8;
        return true;
    }
    /* NaN */
    if (can_read(input_buffer, 3) && (strncmp((const char *) buffer_at_offset(input_buffer), "NaN", 3) == 0)) {
        *item = PyFloat_FromDouble(Py_NAN);
        input_buffer->offset += 3;
        return true;
    }
    /* -NaN */
    if (can_read(input_buffer, 4) && (strncmp((const char *) buffer_at_offset(input_buffer), "-NaN", 4) == 0)) {
        *item = PyFloat_FromDouble(Py_NAN);
        input_buffer->offset += 4;
        return true;
    }

    PyErr_Format(PyExc_ValueError, "Failed to parse value: invalid value\nposition: %d", input_buffer->offset);

    return false;
}

PyObject *pycJSON_Decode(PyObject *self, PyObject *args, PyObject *kwargs) {
    parse_buffer buffer = {0, 0, 0, 0, {0, 0}, 0};
    PyObject *item = NULL;

    const char *value;
    Py_ssize_t buffer_length;
    static const char *kwlist[] = {"s", "object_hook", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s#|O", (char **) kwlist, &value, &buffer_length, &buffer.object_hook)) {
        PyErr_Format(PyExc_TypeError, "Failed to parse JSON: invalid argument, expected str / bytes-like object");
        goto fail;
    }

    if (buffer.object_hook && !PyCallable_Check(buffer.object_hook)) {
        PyErr_Format(PyExc_TypeError, "Failed to parse JSON: object_hook is not callable");
        goto fail;
    }

    if (0 == buffer_length) {
        PyErr_SetString(PyExc_ValueError, "Empty string");
        goto fail;
    }
    buffer.content = (const unsigned char *) value;
    buffer.length = buffer_length;
    buffer.offset = 0;
    buffer.hooks = global_hooks;
    if (!parse_value(&item, buffer_skip_whitespace(skip_utf8_bom(&buffer)))) {
        /* parse failure. ep is set. */
        goto fail;
    }
    if (buffer.offset < buffer.length) {
        PyErr_Format(PyExc_ValueError, "Failed to parse JSON: extra characters at the end\nend position: %d", buffer.offset);
        goto fail;
    }
    if (dconv_s2d_ptr != NULL) {
        dconv_s2d_free(&dconv_s2d_ptr);
    }
    return item;

fail:
    if (value != NULL && !PyErr_Occurred()) {
        Py_ssize_t position = 0;

        if (buffer.offset < buffer.length) {
            position = buffer.offset;
        } else if (buffer.length > 0) {
            position = buffer.length - 1;
        }

        PyErr_Format(PyExc_ValueError, "Failed to parse JSON (position %d)", position);
    }
    if (dconv_s2d_ptr != NULL) {
        dconv_s2d_free(&dconv_s2d_ptr);
    }
    return NULL;
}

PyObject *pycJSON_DecodeFile(PyObject *self, PyObject *args, PyObject *kwargs) {
    Py_RETURN_NOTIMPLEMENTED;
}
