#define PY_SSIZE_T_CLEAN
#include "dconv_wrapper.h"
#include "pycJSON.h"
#include <Python.h>
#include <math.h>
#include <stdbool.h>

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
    internal_hooks hooks;
    bool using_heap;

    /* args to print */
    bool format; /* is this print a formatted print */
    bool skipkeys;
    bool allow_nan;
    const char *item_separator;
    const char *key_separator;
    PyObject *default_func;
} printbuffer;

// forward declaration
static bool print_value(PyObject *item, printbuffer *const output_buffer);
static void *dconv_d2s_ptr = NULL;

/* realloc printbuffer if necessary to have at least "needed" bytes more */
static unsigned char *ensure(printbuffer *const p, size_t needed) {
    assert(p && p->buffer);
    assert(!(p->length > 0 && p->offset >= p->length));

    size_t newsize;

    if (needed > INT_MAX) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for buffer");
        /* sizes bigger than INT_MAX are currently not supported */
        return NULL;
    }

    needed += p->offset + 1;
    if (needed <= p->length) {
        return p->buffer + p->offset;
    }

    /* calculate new buffer size */
    if (needed > (INT_MAX / 2)) {
        /* overflow of int, use INT_MAX if possible */
        if (needed <= INT_MAX) {
            newsize = INT_MAX;
        } else {
            PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for buffer");
            return NULL;
        }
    } else {
        newsize = needed * 2;
    }

    /* reallocate with realloc if available */
    if (p->hooks.reallocate(p, newsize, p->offset + 1) == NULL) {
        // fail
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

/* Render the number nicely from the given item into a string. */
static bool print_number(PyObject *item, printbuffer *const output_buffer) {
    unsigned char *output_pointer = NULL;

    int length = 0;
    size_t i = 0;
    unsigned char number_buffer[32] = {0}; /* temporary buffer to print the number into */
    unsigned char decimal_point = get_decimal_point();

    if (output_buffer == NULL) {
        return false;
    }

    if (PyLong_Check(item)) {
        length = sprintf((char *) number_buffer, "%lld", PyLong_AsLongLong(item));
    } else // it is float
    {
        double d = PyFloat_AS_DOUBLE(item);
        if (PyErr_Occurred()) {
            PyErr_SetString(PyExc_TypeError, "Number is not a float");
            return false;
        }

        /* This checks for NaN and Infinity */
        if (isinf(d)) {
            if (!output_buffer->allow_nan) {
                PyErr_SetString(PyExc_ValueError, "Number is not a valid JSON value: inf or -inf");
                return false;
            }
            if (d < 0)
                length = sprintf((char *) number_buffer, "-Infinity");
            else
                length = sprintf((char *) number_buffer, "Infinity");
        } else if (isnan(d)) {
            if (!output_buffer->allow_nan) {
                PyErr_SetString(PyExc_ValueError, "Number is not a valid JSON value, nan is not allowed");
                return false;
            }
            length = sprintf((char *) number_buffer, "NaN");
        } else {
            /* Try 15 decimal places of precision to avoid nonsignificant nonzero digits */
            // EDITED: 15 -> 16 for python
            // length = sprintf((char *) number_buffer, "%1.16g", d);
            if (dconv_d2s_ptr == NULL) {
                dconv_d2s_init(&dconv_d2s_ptr, NO_FLAGS, "Infinity", "NaN", 'e', -324, 308, 0, 0);
            }
            dconv_d2s(dconv_d2s_ptr, d, (char *) number_buffer, 32, &length);
            // /* Check whether the original double can be recovered */
            // if ((sscanf((char *) number_buffer, "%lg", &test) != 1) || !compare_double((double) test, d)) {
            //     /* If not, print with 17 decimal places of precision */
            //     length = sprintf((char *) number_buffer, "%1.17g", d);
            // }
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
static bool print_string_ptr(const unsigned char *input, printbuffer *const output_buffer) {
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
                case '\n':
                    *output_pointer = 'n';
                    break;
                case '\r':
                    *output_pointer = 'r';
                    break;
                case '\t':
                    *output_pointer = 't';
                    break;
                case '\b':
                    *output_pointer = 'b';
                    break;
                case '\f':
                    *output_pointer = 'f';
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

static bool print_string(PyObject *item, printbuffer *const buffer) {
    return print_string_ptr((const unsigned char *) PyUnicode_AsUTF8(item), buffer);
}

#define insert_seperator(sep, len)                                                  \
    output_pointer = ensure(output_buffer, len);                                    \
    if (output_pointer == NULL) {                                                   \
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for buffer"); \
        return false;                                                               \
    }                                                                               \
    for (int i = 0; i < strlen(sep); i++) *output_pointer++ = sep[i];

/* Render an array to text */
static bool print_array(PyObject *item, printbuffer *const output_buffer) {
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
        Py_DECREF(next_element);
        next_element = PyIter_Next(iter);
        if (next_element) {
            length = (size_t) (output_buffer->format ? 1 : 0) + strlen(output_buffer->item_separator);
            insert_seperator(output_buffer->item_separator, length) if (output_buffer->format) {
                *output_pointer++ = ' ';
            }
            *output_pointer = '\0';
            output_buffer->offset += length;
        }
    }
    Py_DECREF(iter);
    iter = NULL;

    output_pointer = ensure(output_buffer, 2);
    if (output_pointer == NULL) {
        // PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for buffer");
        return false;
    }
    *output_pointer++ = ']';
    *output_pointer = '\0';
    output_buffer->depth--;

    return true;
}

/* Render an object to text. */
static bool print_object(PyObject *item, printbuffer *const output_buffer) {
    unsigned char *output_pointer = NULL;
    size_t length = 0;
    PyObject *iter = PyObject_GetIter(item);
    if (iter == NULL) {
        PyErr_SetString(PyExc_TypeError, "Object is not iterable");
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
                Py_DECREF(str);
                return false;
            }
            Py_DECREF(str);
        } else {
            if (output_buffer->skipkeys) {
                Py_DECREF(next_element);
                next_element = PyIter_Next(iter);

                if (next_element == NULL) {
                    if (PyErr_Occurred()) return false;
                    if (output_buffer->offset > strlen(output_buffer->item_separator)) {
                        bool is_last_sep = 1;
                        for (int i = 0; i < strlen(output_buffer->item_separator); i++) {
                            if (output_buffer->item_separator[i] != (output_buffer->buffer + output_buffer->offset - strlen(output_buffer->item_separator))[i]) {
                                is_last_sep = 0;
                                break;
                            }
                        }
                        if (is_last_sep) output_buffer->offset -= strlen(output_buffer->item_separator);
                    }
                }

                output_pointer = (output_buffer->buffer + output_buffer->offset);
                length = 0;
                goto SKIP_PRINT;
            }
            PyErr_SetString(PyExc_TypeError, "TypeError: Key must be str, None, bool or number");
            return false;
        }

        update_offset(output_buffer);

        length = (size_t) (output_buffer->format ? 1 : 0) + strlen(output_buffer->key_separator);
        insert_seperator(output_buffer->key_separator, length) if (output_buffer->format) {
            *output_pointer++ = '\t';
        }
        output_buffer->offset += length;

        /* print value */
        if (!print_value(PyDict_GetItem(item, next_element), output_buffer)) {
            return false;
        }
        update_offset(output_buffer);

        Py_DECREF(next_element);
        next_element = PyIter_Next(iter);

        /* print comma if not last */
        length = ((size_t) (output_buffer->format ? 1 : 0) + (size_t) (next_element ? strlen(output_buffer->item_separator) : 0));
        output_pointer = ensure(output_buffer, length + 1);
        if (output_pointer == NULL) {
            return false;
        }
        if (next_element) {
            if (PyErr_Occurred()) return false;
            // no need to allocate memory for seperator
            insert_seperator(output_buffer->item_separator, 0)
        }

    SKIP_PRINT:
        if (output_buffer->format) {
            *output_pointer++ = '\n';
        }
        *output_pointer = '\0';
        output_buffer->offset += length;
    }
    Py_DECREF(iter);

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
static bool print_value(PyObject *item, printbuffer *const output_buffer) {
    unsigned char *output = NULL;

    if ((item == NULL) || (output_buffer == NULL)) {
        return false;
    }

    if (item == Py_None) {
        output = ensure(output_buffer, 5);
        if (output == NULL) {
            return false;
        }
        strcpy((char *) output, "null");
        return true;
    }
    if (PyBool_Check(item)) {
        if (PyObject_IsTrue(item)) {
            output = ensure(output_buffer, 5);
            if (output == NULL) {
                return false;
            }
            strcpy((char *) output, "true");
        } else {
            output = ensure(output_buffer, 6);
            if (output == NULL) {
                return false;
            }
            strcpy((char *) output, "false");
        }
        return true;
    } else if (PyUnicode_Check(item))
        return print_string(item, output_buffer);
    else if (PyNumber_Check(item))
        return print_number(item, output_buffer);
    else if (PyList_Check(item) || PyTuple_Check(item))
        return print_array(item, output_buffer);
    else if (PyDict_Check(item))
        return print_object(item, output_buffer);
    else {
        if (output_buffer->default_func) {
            PyObject *re = PyObject_CallFunctionObjArgs(output_buffer->default_func, item, NULL);
            if (re == NULL) {
                PyErr_SetString(PyExc_TypeError, "TypeError: default argument function failed to encode object");
                return false;
            }
            if (!PyUnicode_Check(re)) {
                PyErr_SetString(PyExc_TypeError, "TypeError: default argument function must return string");
                return false;
            }
            return print_string(re, output_buffer);
        }
        PyErr_SetString(PyExc_TypeError, "TypeError: Object of type is not JSON serializable");
        return false;
    }
}


PyObject *pycJSON_Encode(PyObject *self, PyObject *args, PyObject *kwargs) {
    static const size_t default_buffer_size = CJSON_PRINTBUFFER_MAX_STACK_SIZE;
    printbuffer buffer[1];

    memset(buffer, 0, sizeof(buffer));

    unsigned char stack_buffer[CJSON_PRINTBUFFER_MAX_STACK_SIZE];

    static const char *kwlist[] = {"obj", "format", "skipkeys", "allow_nan", "separators", "default", NULL};
    PyObject *arg;
    buffer->format = false;
    buffer->skipkeys = false;
    buffer->allow_nan = true;
    buffer->item_separator = ",";
    buffer->key_separator = ":";
    buffer->default_func = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|ppp(ss)O", (char **) kwlist, &arg, &buffer->format, &buffer->skipkeys, &buffer->allow_nan, &buffer->item_separator, &buffer->key_separator, &buffer->default_func)) {
        if (!PyErr_Occurred()) PyErr_SetString(PyExc_TypeError, "Failed to parse arguments");
        goto fail;
    }
    if (buffer->default_func && !PyCallable_Check(buffer->default_func)) {
        PyErr_SetString(PyExc_TypeError, "default_func must be callable");
        goto fail;
    }
    /* create buffer */
    buffer->buffer = stack_buffer; //(unsigned char *) global_hooks.allocate(default_buffer_size);
    buffer->length = default_buffer_size;
    buffer->hooks = global_hooks;
    if (!print_value(arg, buffer)) {
        if (!PyErr_Occurred()) PyErr_SetString(PyExc_TypeError, "Failed to encode object");
        goto fail;
    }

    update_offset(buffer);

    PyObject *re = PyUnicode_FromString((const char *) buffer->buffer);
    global_hooks.deallocate_self(buffer);
    if (dconv_d2s_ptr != NULL) {
        dconv_d2s_free(&dconv_d2s_ptr);
    }
    return re;

fail:
    if (dconv_d2s_ptr != NULL) {
        dconv_d2s_free(&dconv_d2s_ptr);
    }
    return NULL;
}

PyObject *pycJSON_FileEncode(PyObject *self, PyObject *args, PyObject *kwargs) {
    static const size_t default_buffer_size = CJSON_PRINTBUFFER_MAX_STACK_SIZE;
    printbuffer buffer[1];

    memset(buffer, 0, sizeof(buffer));

    unsigned char stack_buffer[CJSON_PRINTBUFFER_MAX_STACK_SIZE];

    static const char *kwlist[] = {"obj", "fp", "format", "skipkeys", "allow_nan", "separators", "default", NULL};
    PyObject *arg;
    buffer->format = false;
    buffer->skipkeys = false;
    buffer->allow_nan = true;
    buffer->item_separator = ",";
    buffer->key_separator = ":";
    buffer->default_func = NULL;
    PyObject *file_obj;
    PyObject *write_method;
    PyObject *re = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|ppp(ss)O", (char **) kwlist, &arg, &file_obj, &buffer->format, &buffer->skipkeys, &buffer->allow_nan, &buffer->item_separator, &buffer->key_separator, &buffer->default_func)) {
        if (!PyErr_Occurred()) PyErr_SetString(PyExc_TypeError, "Failed to parse arguments");
        goto fail;
    }

    write_method = PyObject_GetAttrString(file_obj, "write");
    if (!PyCallable_Check(write_method)) {
        PyErr_SetString(PyExc_TypeError, "'write' method is not callable");
        goto fail;
    }

    if (buffer->default_func && !PyCallable_Check(buffer->default_func)) {
        PyErr_SetString(PyExc_TypeError, "default_func must be callable");
        goto fail;
    }
    /* create buffer */
    buffer->buffer = stack_buffer; //(unsigned char *) global_hooks.allocate(default_buffer_size);
    buffer->length = default_buffer_size;
    buffer->hooks = global_hooks;
    if (!print_value(arg, buffer)) {
        if (!PyErr_Occurred()) PyErr_SetString(PyExc_TypeError, "Failed to encode object");
        goto fail;
    }

    update_offset(buffer);

    re = PyUnicode_FromString((const char *) buffer->buffer);
    global_hooks.deallocate_self(buffer);


    if (!PyUnicode_Check(re)) {
        PyErr_SetString(PyExc_TypeError, "file content must be a string");
        goto fail;
    }

    PyObject *argtuple = PyTuple_Pack(1, re);
    if (argtuple == NULL) {
        PyErr_SetString(PyErr_SetString, "Failed to pack the result, required for writing to the file");
        goto fail;
    }

    PyObject *file_contents = PyObject_CallObject(write_method, argtuple);
    if (file_contents == NULL) {
        PyErr_SetString(PyErr_SetString, "Failed to write result to the file");
        goto fail;
    }

    Py_XDECREF(file_contents);
    Py_XDECREF(write_method);
    Py_XDECREF(re);
    Py_XDECREF(argtuple);

    Py_RETURN_NONE;

fail:
    Py_XDECREF(write_method);
    Py_XDECREF(re);

    return NULL;
}

static void CJSON_CDECL internal_free(printbuffer *buffer) {
    if (buffer->using_heap) {
        PyMem_Free(buffer->buffer);
    }
    buffer->buffer = NULL;
    buffer->length = 0;
}

static void *CJSON_CDECL internal_realloc(printbuffer *buffer, size_t size, size_t copy_len) {
    if (size > INT_MAX) {
        if (buffer->buffer != NULL && buffer->using_heap) {
            PyMem_Free(buffer->buffer);
        }
        buffer->buffer = NULL;
        buffer->length = 0;
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for buffer: size is over INT_MAX");
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
    unsigned char *newbuffer = (unsigned char *) PyMem_Malloc(size);
    if (newbuffer == NULL) {
        // fail
        if (buffer->using_heap && buffer->buffer != NULL) {
            PyMem_Free(buffer->buffer);
        }
        buffer->buffer = NULL;
        buffer->length = 0;
        PyErr_SetString(PyExc_MemoryError, "Failed to reallocate memory for buffer");
        return NULL;
    }
    memcpy(newbuffer, buffer->buffer, cjson_min(copy_len, size));
    if (buffer->using_heap && buffer->buffer != NULL) {
        PyMem_Free(buffer->buffer);
    }
    buffer->buffer = newbuffer;
    buffer->length = size;
    buffer->using_heap = true;
    return newbuffer;
}
