#include "pycJSON.h"
#include <Python.h>
#include <float.h>
#include <math.h>

#define true cJSON_True
#define false cJSON_False

#define cjson_min(a, b) (((a) < (b)) ? (a) : (b))

#define CJSON_PRINTBUFFER_MAX_STACK_SIZE (1024 * 256)

typedef struct printbuffer printbuffer;

typedef struct internal_hooks {
    void(CJSON_CDECL *deallocate_self)(printbuffer *);
    void *(CJSON_CDECL *reallocate)(printbuffer *, size_t size, size_t copy_len);
} internal_hooks;

static void CJSON_CDECL internal_free(printbuffer *);

static void *CJSON_CDECL internal_realloc(printbuffer *, size_t, size_t);

static internal_hooks global_hooks = {internal_free, internal_realloc};

typedef struct printbuffer {
    unsigned char *buffer;
    size_t length;
    size_t offset;
    size_t depth; /* current nesting depth (for formatted printing) */
    cJSON_bool noalloc;
    cJSON_bool format; /* is this print a formatted print */
    internal_hooks hooks;
    cJSON_bool using_heap;
} printbuffer;

static cJSON_bool print_value(const PyObject *const item, printbuffer *const output_buffer);

/* realloc printbuffer if necessary to have at least "needed" bytes more */
static unsigned char *ensure(printbuffer *const p, size_t needed) {
    size_t newsize = 0;

    if ((p == NULL) || (p->buffer == NULL)) {
        return NULL;
    }

    if ((p->length > 0) && (p->offset >= p->length)) {
        /* make sure that offset is valid */
        return NULL;
    }

    if (needed > INT_MAX) {
        /* sizes bigger than INT_MAX are currently not supported */
        return NULL;
    }

    needed += p->offset + 1;
    if (needed <= p->length) {
        return p->buffer + p->offset;
    }

    if (p->noalloc) {
        return NULL;
    }

    /* calculate new buffer size */
    if (needed > (INT_MAX / 2)) {
        /* overflow of int, use INT_MAX if possible */
        if (needed <= INT_MAX) {
            newsize = INT_MAX;
        } else {
            return NULL;
        }
    } else {
        newsize = needed * 2;
    }

    /* reallocate with realloc if available */
    p->hooks.reallocate(p, newsize, p->offset + 1);
    if (p->buffer == NULL) {
        return NULL;
    }

    return p->buffer + p->offset;
}

/* calculate the new length of the string in a printbuffer and update the offset */
static void update_offset(printbuffer *const buffer) {
    const unsigned char *buffer_pointer = NULL;
    if ((buffer == NULL) || (buffer->buffer == NULL)) {
        return;
    }
    buffer_pointer = buffer->buffer + buffer->offset;

    buffer->offset += strlen((const char *) buffer_pointer);
}

/* get the decimal point character of the current locale */
static unsigned char get_decimal_point(void) {
#ifdef ENABLE_LOCALES
    struct lconv *lconv = localeconv();
    return (unsigned char) lconv->decimal_point[0];
#else
    return '.';
#endif
}

/* securely comparison of floating-point variables */
static cJSON_bool compare_double(double a, double b) {
    double maxVal = fabs(a) > fabs(b) ? fabs(a) : fabs(b);
    return (fabs(a - b) <= maxVal * DBL_EPSILON);
}

/* Render the number nicely from the given item into a string. */
static cJSON_bool print_number(PyObject *const item, printbuffer *const output_buffer) {
    unsigned char *output_pointer = NULL;

    int length = 0;
    size_t i = 0;
    unsigned char number_buffer[26] = {0}; /* temporary buffer to print the number into */
    unsigned char decimal_point = get_decimal_point();
    double test = 0.0;

    if (output_buffer == NULL) {
        return false;
    }

    if (PyLong_Check(item)) {
        length = sprintf((char *) number_buffer, "%lld", PyLong_AsLongLong(item));
    } else // it is float
    {
        double d = PyFloat_AsDouble(item);
        if (PyErr_Occurred()) {
            PyErr_SetString(PyExc_TypeError, "Number is not a float");
            return false;
        }
        /* This checks for NaN and Infinity */
        if (isinf(d)) {
            length = sprintf((char *) number_buffer, "Infinity");
        } else if (isnan(d)) {
            length = sprintf((char *) number_buffer, "NaN");
        } else {
            /* Try 15 decimal places of precision to avoid nonsignificant nonzero digits */
            // EDITED: 15 -> 16 for python
            length = sprintf((char *) number_buffer, "%1.16g", d);

            /* Check whether the original double can be recovered */
            if ((sscanf((char *) number_buffer, "%lg", &test) != 1) || !compare_double((double) test, d)) {
                /* If not, print with 17 decimal places of precision */
                length = sprintf((char *) number_buffer, "%1.17g", d);
            }
        }
    }

    /* sprintf failed or buffer overrun occurred */
    if ((length < 0) || (length > (int) (sizeof(number_buffer) - 1))) {
        return false;
    }

    /* reserve appropriate space in the output */
    output_pointer = ensure(output_buffer, (size_t) length + sizeof(""));
    if (output_pointer == NULL) {
        return false;
    }

    /* copy the printed number to the output and replace locale
     * dependent decimal point with '.' */
    for (i = 0; i < ((size_t) length); i++) {
        if (number_buffer[i] == decimal_point) {
            output_pointer[i] = '.';
            continue;
        }

        output_pointer[i] = number_buffer[i];
    }
    output_pointer[i] = '\0';

    output_buffer->offset += (size_t) length;

    return true;
}

/* Render the cstring provided to an escaped version that can be printed. */
static cJSON_bool print_string_ptr(const unsigned char *const input, printbuffer *const output_buffer) {
    const unsigned char *input_pointer = NULL;
    unsigned char *output = NULL;
    unsigned char *output_pointer = NULL;
    size_t output_length = 0;
    /* numbers of additional characters needed for escaping */
    size_t escape_characters = 0;

    if (output_buffer == NULL) {
        return false;
    }

    /* empty string */
    if (input == NULL) {
        output = ensure(output_buffer, sizeof("\"\""));
        if (output == NULL) {
            return false;
        }
        strcpy((char *) output, "\"\"");

        return true;
    }

    /* set "flag" to 1 if something needs to be escaped */
    for (input_pointer = input; *input_pointer; input_pointer++) {
        switch (*input_pointer) {
            case '\"':
            case '\\':
            case '\b':
            case '\f':
            case '\n':
            case '\r':
            case '\t':
                /* one character escape sequence */
                escape_characters++;
                break;
            default:
                if (*input_pointer < 32) {
                    /* UTF-16 escape sequence uXXXX */
                    escape_characters += 5;
                }
                break;
        }
    }
    output_length = (size_t) (input_pointer - input) + escape_characters;

    output = ensure(output_buffer, output_length + sizeof("\"\""));
    if (output == NULL) {
        return false;
    }

    /* no characters have to be escaped */
    if (escape_characters == 0) {
        output[0] = '\"';
        memcpy(output + 1, input, output_length);
        output[output_length + 1] = '\"';
        output[output_length + 2] = '\0';

        return true;
    }

    output[0] = '\"';
    output_pointer = output + 1;
    /* copy the string */
    for (input_pointer = input; *input_pointer != '\0'; (void) input_pointer++, output_pointer++) {
        if ((*input_pointer > 31) && (*input_pointer != '\"') && (*input_pointer != '\\')) {
            /* normal character, copy */
            *output_pointer = *input_pointer;
        } else {
            /* character needs to be escaped */
            *output_pointer++ = '\\';
            switch (*input_pointer) {
                case '\\':
                    *output_pointer = '\\';
                    break;
                case '\"':
                    *output_pointer = '\"';
                    break;
                case '\b':
                    *output_pointer = 'b';
                    break;
                case '\f':
                    *output_pointer = 'f';
                    break;
                case '\n':
                    *output_pointer = 'n';
                    break;
                case '\r':
                    *output_pointer = 'r';
                    break;
                case '\t':
                    *output_pointer = 't';
                    break;
                default:
                    /* escape and print as unicode codepoint */
                    sprintf((char *) output_pointer, "u%04x", *input_pointer);
                    output_pointer += 4;
                    break;
            }
        }
    }
    output[output_length + 1] = '\"';
    output[output_length + 2] = '\0';

    return true;
}

static cJSON_bool print_string(PyObject *const item, printbuffer *const buffer) {
    return print_string_ptr((const unsigned char *) PyUnicode_AsUTF8(item), buffer);
}

/* Render an array to text */
static cJSON_bool print_array(const PyObject *const item, printbuffer *const output_buffer) {
    unsigned char *output_pointer = NULL;
    size_t length = 0;
    PyObject *iter = PyObject_GetIter(item);
    if (iter == NULL) {
        PyErr_SetString(PyExc_TypeError, "TypeError: Object is not iterable");
        return false;
    }
    if (output_buffer == NULL) {
        return false;
    }

    /* Compose the output array. */
    /* opening square bracket */
    output_pointer = ensure(output_buffer, 1);
    if (output_pointer == NULL) {
        return false;
    }

    *output_pointer = '[';
    output_buffer->offset++;
    output_buffer->depth++;

    PyObject *next_element = PyIter_Next(iter);
    while (next_element) {
        if (!print_value(next_element, output_buffer)) {
            return false;
        }
        update_offset(output_buffer);
        Py_DecRef(next_element);
        next_element = PyIter_Next(iter);
        if (next_element) {
            length = (size_t) (output_buffer->format ? 2 : 1);
            output_pointer = ensure(output_buffer, length + 1);
            if (output_pointer == NULL) {
                return false;
            }
            *output_pointer++ = ',';
            if (output_buffer->format) {
                *output_pointer++ = ' ';
            }
            *output_pointer = '\0';
            output_buffer->offset += length;
        }
    }
    Py_DecRef(iter);
    iter = NULL;

    output_pointer = ensure(output_buffer, 2);
    if (output_pointer == NULL) {
        return false;
    }
    *output_pointer++ = ']';
    *output_pointer = '\0';
    output_buffer->depth--;

    return true;
}

/* Render an object to text. */
static cJSON_bool print_object(const PyObject *const item, printbuffer *const output_buffer) {
    unsigned char *output_pointer = NULL;
    size_t length = 0;
    PyObject *iter = PyObject_GetIter(item);
    if (iter == NULL) {
        PyErr_SetString(PyExc_TypeError, "TypeError: Object is not iterable");
        return false;
    }

    if (output_buffer == NULL) {
        return false;
    }

    /* Compose the output: */
    length = (size_t) (output_buffer->format ? 2 : 1); /* fmt: {\n */
    output_pointer = ensure(output_buffer, length + 1);
    if (output_pointer == NULL) {
        return false;
    }

    *output_pointer++ = '{';
    output_buffer->depth++;
    if (output_buffer->format) {
        *output_pointer++ = '\n';
    }
    output_buffer->offset += length;
    PyObject *next_element = PyIter_Next(iter);
    while (next_element) {
        if (output_buffer->format) {
            size_t i;
            output_pointer = ensure(output_buffer, output_buffer->depth);
            if (output_pointer == NULL) {
                return false;
            }
            for (i = 0; i < output_buffer->depth; i++) {
                *output_pointer++ = '\t';
            }
            output_buffer->offset += output_buffer->depth;
        }

        /* print key */
        if (PyUnicode_Check(next_element)) {
            if (!print_string(next_element, output_buffer))
                return false;
        } else if (next_element == Py_None) {
            unsigned char *output = ensure(output_buffer, 5);
            if (output == NULL) {
                return false;
            }
            strcpy((char *) output, "null");
        } else if (PyBool_Check(next_element)) {
            if (PyObject_IsTrue(next_element)) {
                unsigned char *output = ensure(output_buffer, 5);
                if (output == NULL) {
                    return false;
                }
                strcpy((char *) output, "true");
            } else {
                unsigned char *output = ensure(output_buffer, 6);
                if (output == NULL) {
                    return false;
                }
                strcpy((char *) output, "false");
            }
        } else if (PyNumber_Check(next_element)) {
            PyObject *str = PyObject_Str(next_element);
            if (!print_string(str, output_buffer)) {
                Py_DecRef(str);
                return false;
            }
            Py_DecRef(str);
        } else {
            PyErr_SetString(PyExc_TypeError, "TypeError: Key must be ");
            Py_RETURN_NONE;
        }

        update_offset(output_buffer);

        length = (size_t) (output_buffer->format ? 2 : 1);
        output_pointer = ensure(output_buffer, length);
        if (output_pointer == NULL) {
            return false;
        }
        *output_pointer++ = ':';
        if (output_buffer->format) {
            *output_pointer++ = '\t';
        }
        output_buffer->offset += length;

        /* print value */
        if (!print_value(PyDict_GetItem(item, next_element), output_buffer)) {
            return false;
        }
        update_offset(output_buffer);

        Py_DecRef(next_element);
        next_element = PyIter_Next(iter);

        /* print comma if not last */
        length = ((size_t) (output_buffer->format ? 1 : 0) + (size_t) (next_element ? 1 : 0));
        output_pointer = ensure(output_buffer, length + 1);
        if (output_pointer == NULL) {
            return false;
        }
        if (next_element) {
            *output_pointer++ = ',';
        }

        if (output_buffer->format) {
            *output_pointer++ = '\n';
        }
        *output_pointer = '\0';
        output_buffer->offset += length;
    }
    Py_DecRef(iter);

    output_pointer = ensure(output_buffer, output_buffer->format ? (output_buffer->depth + 1) : 2);
    if (output_pointer == NULL) {
        return false;
    }
    if (output_buffer->format) {
        size_t i;
        for (i = 0; i < (output_buffer->depth - 1); i++) {
            *output_pointer++ = '\t';
        }
    }
    *output_pointer++ = '}';
    *output_pointer = '\0';
    output_buffer->depth--;

    return true;
}

/* Render a value to text. */
static cJSON_bool print_value(const PyObject *const item, printbuffer *const output_buffer) {
    unsigned char *output = NULL;

    if ((item == NULL) || (output_buffer == NULL)) {
        return cJSON_False;
    }

    if (item == Py_None) {
        output = ensure(output_buffer, 5);
        if (output == NULL) {
            return cJSON_False;
        }
        strcpy((char *) output, "null");
        return cJSON_True;
    }
    if (PyBool_Check(item)) {
        if (PyObject_IsTrue(item)) {
            output = ensure(output_buffer, 5);
            if (output == NULL) {
                return cJSON_False;
            }
            strcpy((char *) output, "true");
        } else {
            output = ensure(output_buffer, 6);
            if (output == NULL) {
                return cJSON_False;
            }
            strcpy((char *) output, "false");
        }
        return cJSON_True;
    } else if (PyUnicode_Check(item))
        return print_string(item, output_buffer);
    else if (PyNumber_Check(item))
        return print_number(item, output_buffer);
    else if (PyList_Check(item) || PyTuple_Check(item))
        return print_array(item, output_buffer);
    else if (PyDict_Check(item))
        return print_object(item, output_buffer);
    else {
        PyErr_SetString(PyExc_TypeError, "TypeError: Object of type is not JSON serializable"); // TODO ?
        Py_RETURN_NONE;
    }

    // No cJSON_Raw?
    // case cJSON_Raw:
    // {
    //     size_t raw_length = 0;
    //     if (item->valuestring == NULL)
    //     {
    //         return cJSON_False;
    //     }

    //     raw_length = strlen(item->valuestring) + sizeof("");
    //     output = ensure(output_buffer, raw_length);
    //     if (output == NULL)
    //     {
    //         return cJSON_False;
    //     }
    //     memcpy(output, item->valuestring, raw_length);
    //     return true;
    // }
}


PyObject *pycJSON_Encode(PyObject *self, PyObject *args, PyObject *kwargs) {
    static const size_t default_buffer_size = CJSON_PRINTBUFFER_MAX_STACK_SIZE;
    printbuffer buffer[1];
    unsigned char *printed = NULL;

    memset(buffer, 0, sizeof(buffer));

    unsigned char stack_buffer[CJSON_PRINTBUFFER_MAX_STACK_SIZE];

    /* create buffer */
    buffer->buffer = stack_buffer; //(unsigned char *) global_hooks.allocate(default_buffer_size);
    buffer->length = default_buffer_size;
    buffer->format = kwargs && PyDict_Contains(kwargs, PyUnicode_FromString("format")) && PyObject_IsTrue(PyDict_GetItem(kwargs, PyUnicode_FromString("format")));
    buffer->hooks = global_hooks;
    if (buffer->buffer == NULL) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for buffer");
        Py_RETURN_NONE;
    }
    PyObject *arg = PyTuple_GET_ITEM(args, 0);
    if (!print_value(arg, buffer)) {
        PyErr_SetString(PyExc_TypeError, "Failed to encode object");
        Py_RETURN_NONE;
    }

    update_offset(buffer);

    PyObject *re = PyUnicode_FromString(buffer->buffer);
    global_hooks.deallocate_self(buffer);
    return re;
}

PyObject *pycJSON_FileEncode(PyObject *self, PyObject *args, PyObject *kwargs) {
    Py_RETURN_NOTIMPLEMENTED;
}

static void CJSON_CDECL internal_free(printbuffer *buffer) {
    if (buffer->using_heap) {
        free(buffer->buffer);
    }
    buffer->buffer = NULL;
    buffer->length = 0;
}

static void *CJSON_CDECL internal_realloc(printbuffer *buffer, size_t size, size_t copy_len) {
    if (size > INT_MAX) {
        if (buffer->buffer != NULL && buffer->using_heap) {
            free(buffer->buffer);
        }
        buffer->buffer = NULL;
        buffer->length = 0;
        return NULL;
    }
    if (size <= buffer->length) {
        return buffer->buffer;
    }
    if (size < CJSON_PRINTBUFFER_MAX_STACK_SIZE && !buffer->using_heap) {
        // no copy, using current stack buffer
        if (buffer->buffer == NULL) {
            // should not reach here
            buffer->length = 0;
            return NULL;
        }
        buffer->length = size;
        return buffer->buffer;
    }
    unsigned char *newbuffer = (unsigned char *) malloc(size);
    if (newbuffer == NULL) {
        // fail
        if (buffer->using_heap && buffer->buffer != NULL) {
            free(buffer->buffer);
        }
        buffer->buffer = NULL;
        buffer->length = 0;
        return NULL;
    }
    memcpy(newbuffer, buffer->buffer, cjson_min(copy_len, size));
    if (buffer->using_heap && buffer->buffer != NULL) {
        free(buffer->buffer);
    }
    buffer->buffer = newbuffer;
    buffer->length = size;
    buffer->using_heap = true;
    return newbuffer;
}
