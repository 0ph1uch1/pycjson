#include "pycJSON.h"
#include <Python.h>
#include <math.h>

/* check if the given size is left to read in a given parse buffer (starting with 1) */
#define can_read(buffer, size) ((buffer != NULL) && (((buffer)->offset + size) <= (buffer)->length))
/* check if the buffer can be accessed at the given index (starting with 0) */
#define can_access_at_index(buffer, index) ((buffer != NULL) && (((buffer)->offset + index) < (buffer)->length))
#define cannot_access_at_index(buffer, index) (!can_access_at_index(buffer, index))
/* get a pointer to the buffer at the position */
#define buffer_at_offset(buffer) ((buffer)->content + (buffer)->offset)

typedef struct internal_hooks {
    void *(CJSON_CDECL *allocate)(size_t size);
    void(CJSON_CDECL *deallocate)(void *pointer);
    void *(CJSON_CDECL *reallocate)(void *pointer, size_t size);
} internal_hooks;

#define internal_malloc malloc
#define internal_free free
#define internal_realloc realloc

static internal_hooks global_hooks = {internal_malloc, internal_free, internal_realloc};

typedef struct
{
    const unsigned char *content;
    size_t length;
    size_t offset;
    size_t depth; /* How deeply nested (in arrays/objects) is the input at the current offset. */
    internal_hooks hooks;
} parse_buffer;

typedef struct cJSON_Array_Item {
    PyObject **buffer;
    struct cJSON_Array_Item *next;
} cJSON_Array_Item;

typedef struct cJSON_Dict_Item {
    PyObject **key_buffer;
    PyObject **value_buffer;
    struct cJSON_Dict_Item *next;
} cJSON_Dict_Item;

/* get the decimal point character of the current locale */
static unsigned char get_decimal_point(void) {
#ifdef ENABLE_LOCALES
    struct lconv *lconv = localeconv();
    return (unsigned char) lconv->decimal_point[0];
#else
    return '.';
#endif
}

static cJSON_bool parse_value(PyObject **item, parse_buffer *const input_buffer);

/* parse 4 digit hexadecimal number */
static unsigned parse_hex4(const unsigned char *const input) {
    unsigned int h = 0;
    size_t i = 0;

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

/* Parse the input text into an unescaped cinput, and populate item. */
static cJSON_bool parse_string(PyObject **item, parse_buffer *const input_buffer) {
    const unsigned char *input_pointer = buffer_at_offset(input_buffer) + 1;
    const unsigned char *input_end = buffer_at_offset(input_buffer) + 1;
    unsigned char *output_pointer = NULL;
    unsigned char *output = NULL;

    /* not a string */
    if (buffer_at_offset(input_buffer)[0] != '\"') {
        goto fail;
    }

    {
        /* calculate approximate size of the output (overestimate) */
        size_t allocation_length = 0;
        size_t skipped_bytes = 0;
        while (((size_t) (input_end - input_buffer->content) < input_buffer->length) && (*input_end != '\"')) {
            /* is escape sequence */
            if (input_end[0] == '\\') {
                if ((size_t) (input_end + 1 - input_buffer->content) >= input_buffer->length) {
                    /* prevent buffer overflow when last input character is a backslash */
                    goto fail;
                }
                skipped_bytes++;
                input_end++;
            }
            input_end++;
        }
        if (((size_t) (input_end - input_buffer->content) >= input_buffer->length) || (*input_end != '\"')) {
            goto fail; /* string ended unexpectedly */
        }

        /* This is at most how much we need for the output */
        allocation_length = (size_t) (input_end - buffer_at_offset(input_buffer)) - skipped_bytes;
        output = (unsigned char *) input_buffer->hooks.allocate(allocation_length + sizeof(""));
        if (output == NULL) {
            goto fail; /* allocation failure */
        }
    }

    output_pointer = output;
    /* loop through the string literal */
    while (input_pointer < input_end) {
        if (*input_pointer != '\\') {
            *output_pointer++ = *input_pointer++;
        }
        /* escape sequence */
        else {
            unsigned char sequence_length = 2;
            if ((input_end - input_pointer) < 1) {
                goto fail;
            }

            switch (input_pointer[1]) {
                case 'b':
                    *output_pointer++ = '\b';
                    break;
                case 'f':
                    *output_pointer++ = '\f';
                    break;
                case 'n':
                    *output_pointer++ = '\n';
                    break;
                case 'r':
                    *output_pointer++ = '\r';
                    break;
                case 't':
                    *output_pointer++ = '\t';
                    break;
                case '\"':
                case '\\':
                case '/':
                    *output_pointer++ = input_pointer[1];
                    break;

                /* UTF-16 literal */
                case 'u':
                    sequence_length = utf16_literal_to_utf8(input_pointer, input_end, &output_pointer);
                    if (sequence_length == 0) {
                        /* failed to convert UTF16-literal to UTF-8 */
                        goto fail;
                    }
                    break;

                default:
                    goto fail;
            }
            input_pointer += sequence_length;
        }
    }

    /* zero terminate the output */
    *output_pointer = '\0';

    *item = PyUnicode_FromString((char *) output);

    input_buffer->offset = (size_t) (input_end - input_buffer->content);
    input_buffer->offset++;

    return true;

fail:
    if (output != NULL) {
        input_buffer->hooks.deallocate(output);
    }

    if (input_pointer != NULL) {
        input_buffer->offset = (size_t) (input_pointer - input_buffer->content);
    }

    return false;
}

/* Build an array from input text. */
static cJSON_bool parse_array(PyObject **item, parse_buffer *const input_buffer) {
    cJSON_Array_Item *head = NULL; /* head of the linked list */
    cJSON_Array_Item *current_item = NULL;
    int count = 0;

    if (input_buffer->depth >= CJSON_NESTING_LIMIT) {
        return false; /* to deeply nested */
    }
    input_buffer->depth++;

    if (buffer_at_offset(input_buffer)[0] != '[') {
        /* not an array */
        goto fail;
    }

    input_buffer->offset++;
    buffer_skip_whitespace(input_buffer);
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == ']')) {
        /* empty array */
        *item = PyList_New(0);
        input_buffer->offset++;
        return true;
    }

    /* check if we skipped to the end of the buffer */
    if (cannot_access_at_index(input_buffer, 0)) {
        input_buffer->offset--;
        goto fail;
    }
    /* step back to character in front of the first element */
    input_buffer->offset--;
    /* loop through the comma separated array elements */
    do {
        /* allocate next item */
        cJSON_Array_Item *new_item = input_buffer->hooks.allocate(sizeof(cJSON_Array_Item));
        new_item->next = NULL;
        count++;
        if (new_item == NULL) {
            goto fail; /* allocation failure */
        }
        new_item->buffer = input_buffer->hooks.allocate(sizeof(PyObject *));
        if (new_item->buffer == NULL) {
            goto fail; /* allocation failure */
        }

        /* attach next item to list */
        if (head == NULL) {
            /* start the linked list */
            current_item = head = new_item;
        } else {
            /* add to the end and advance */
            current_item->next = new_item;
            current_item = new_item;
        }

        /* parse next value */
        input_buffer->offset++;
        buffer_skip_whitespace(input_buffer);
        if (!parse_value(current_item->buffer, input_buffer)) {
            goto fail; /* failed to parse value */
        }
        buffer_skip_whitespace(input_buffer);
    } while (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == ','));
    if (cannot_access_at_index(input_buffer, 0) || buffer_at_offset(input_buffer)[0] != ']') {
        goto fail; /* expected end of array */
    }
    *item = PyList_New(count);
    if (*item == NULL) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for list");
        goto fail;
    }
    int i = 0;
    for (current_item = head; current_item;) {
        PyList_SetItem(*item, i++, *(current_item->buffer));
        cJSON_Array_Item *next = current_item->next;
        input_buffer->hooks.deallocate(current_item->buffer);
        input_buffer->hooks.deallocate(current_item);
        current_item = next;
    }

success:
    input_buffer->depth--;

    input_buffer->offset++;

    return true;

fail:
    while (head) {
        cJSON_Array_Item *next = head->next;
        input_buffer->hooks.deallocate(head->buffer);
        input_buffer->hooks.deallocate(head);
        head = next;
    }

    return false;
}

/* Parse the input text to generate a number, and populate the result into item. */
static cJSON_bool parse_number(PyObject **item, parse_buffer *const input_buffer) {
    double number = 0;
    unsigned char *after_end = NULL;
    unsigned char number_c_string[64];
    unsigned char decimal_point = get_decimal_point();
    size_t i = 0;

    if ((input_buffer == NULL) || (input_buffer->content == NULL)) {
        return false;
    }

    /* copy the number into a temporary buffer and replace '.' with the decimal point
     * of the current locale (for strtod)
     * This also takes care of '\0' not necessarily being available for marking the end of the input */
    for (i = 0; (i < (sizeof(number_c_string) - 1)) && can_access_at_index(input_buffer, i); i++) {
        switch (buffer_at_offset(input_buffer)[i]) {
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
            case 'e':
            case 'E':
                number_c_string[i] = buffer_at_offset(input_buffer)[i];
                break;

            case '.':
                number_c_string[i] = decimal_point;
                break;

            default:
                goto loop_end;
        }
    }
loop_end:
    number_c_string[i] = '\0';

    number = strtod((const char *) number_c_string, (char **) &after_end);
    if (number_c_string == after_end) {
        return false; /* parse_error */
    }

    // if it is int
    if (number == (double) (long long) number)
        *item = PyLong_FromLongLong((long long) number);
    else
        *item = PyFloat_FromDouble(number);

    input_buffer->offset += (size_t) (after_end - number_c_string);
    return true;
}

/* Build an object from the text. */
static cJSON_bool parse_object(PyObject **item, parse_buffer *const input_buffer) {
    cJSON_Dict_Item *head = NULL; /* linked list head */
    cJSON_Dict_Item *current_item = NULL;

    if (input_buffer->depth >= CJSON_NESTING_LIMIT) {
        return false; /* to deeply nested */
    }
    input_buffer->depth++;

    if (cannot_access_at_index(input_buffer, 0) || (buffer_at_offset(input_buffer)[0] != '{')) {
        goto fail; /* not an object */
    }

    input_buffer->offset++;
    buffer_skip_whitespace(input_buffer);
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '}')) {
        /* empty object */
        *item = PyDict_New();
        input_buffer->offset++;
        return true;
    }

    /* check if we skipped to the end of the buffer */
    if (cannot_access_at_index(input_buffer, 0)) {
        input_buffer->offset--;
        goto fail;
    }

    /* step back to character in front of the first element */
    input_buffer->offset--;
    /* loop through the comma separated array elements */
    do {
        /* allocate next item */
        cJSON_Dict_Item *new_item = input_buffer->hooks.allocate(sizeof(cJSON_Dict_Item));
        new_item->key_buffer = input_buffer->hooks.allocate(sizeof(PyObject *));
        new_item->value_buffer = input_buffer->hooks.allocate(sizeof(PyObject *));
        new_item->next = NULL;
        if (new_item == NULL) {
            goto fail; /* allocation failure */
        }

        /* attach next item to list */
        if (head == NULL) {
            /* start the linked list */
            current_item = head = new_item;
        } else {
            /* add to the end and advance */
            current_item->next = new_item;
            current_item = new_item;
        }

        /* parse the name of the child */
        input_buffer->offset++;
        buffer_skip_whitespace(input_buffer);
        if (!parse_string(new_item->key_buffer, input_buffer)) {
            goto fail; /* failed to parse name */
        }
        buffer_skip_whitespace(input_buffer);

        if (cannot_access_at_index(input_buffer, 0) || (buffer_at_offset(input_buffer)[0] != ':')) {
            goto fail; /* invalid object */
        }

        /* parse the value */
        input_buffer->offset++;
        buffer_skip_whitespace(input_buffer);
        if (!parse_value(new_item->value_buffer, input_buffer)) {
            goto fail; /* failed to parse value */
        }
        buffer_skip_whitespace(input_buffer);
    } while (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == ','));

    if (cannot_access_at_index(input_buffer, 0) || (buffer_at_offset(input_buffer)[0] != '}')) {
        goto fail; /* expected end of object */
    }

    *item = PyDict_New();
    for (current_item = head; current_item;) {
        cJSON_Dict_Item *next = current_item->next;
        PyDict_SetItem(*item, *(current_item->key_buffer), *(current_item->value_buffer));
        input_buffer->hooks.deallocate(current_item->key_buffer);
        input_buffer->hooks.deallocate(current_item->value_buffer);
        input_buffer->hooks.deallocate(current_item);
        current_item = next;
    }

success:
    input_buffer->depth--;

    input_buffer->offset++;
    return true;

fail:
    while (head) {
        cJSON_Dict_Item *next = head->next;
        input_buffer->hooks.deallocate(head->key_buffer);
        input_buffer->hooks.deallocate(head->value_buffer);
        input_buffer->hooks.deallocate(head);
        head = next;
    }

    return false;
}

/* Parser core - when encountering text, process appropriately. */
static cJSON_bool parse_value(PyObject **item, parse_buffer *const input_buffer) {
    if ((input_buffer == NULL) || (input_buffer->content == NULL)) {
        return false; /* no input */
    }

    /* parse the different types of values */
    /* null */
    if (can_read(input_buffer, 4) && (strncmp((const char *) buffer_at_offset(input_buffer), "null", 4) == 0)) {
        *item = Py_None;
        input_buffer->offset += 4;
        return true;
    }
    /* false */
    if (can_read(input_buffer, 5) && (strncmp((const char *) buffer_at_offset(input_buffer), "false", 5) == 0)) {
        *item = Py_NewRef(Py_False);
        input_buffer->offset += 5;
        return true;
    }
    /* true */
    if (can_read(input_buffer, 4) && (strncmp((const char *) buffer_at_offset(input_buffer), "true", 4) == 0)) {
        *item = Py_NewRef(Py_True);
        input_buffer->offset += 4;
        return true;
    }
    if (can_read(input_buffer, 8) && (strncmp((const char *) buffer_at_offset(input_buffer), "Infinity", 8) == 0)) {
        *item = PyFloat_FromDouble(INFINITY);
        input_buffer->offset += 8;
        return true;
    }
    if (can_read(input_buffer, 9) && (strncmp((const char *) buffer_at_offset(input_buffer), "-Infinity", 9) == 0)) {
        *item = PyFloat_FromDouble(-INFINITY);
        input_buffer->offset += 9;
        return true;
    }
    if (can_read(input_buffer, 3) && (strncmp((const char *) buffer_at_offset(input_buffer), "NaN", 3) == 0)) {
        *item = PyFloat_FromDouble(NAN);
        input_buffer->offset += 3;
        return true;
    }
    if (can_read(input_buffer, 4) && (strncmp((const char *) buffer_at_offset(input_buffer), "-NaN", 4) == 0)) {
        *item = PyFloat_FromDouble(NAN);
        input_buffer->offset += 4;
        return true;
    }
    /* string */
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '\"')) {
        return parse_string(item, input_buffer);
    }
    /* number */
    if (can_access_at_index(input_buffer, 0) && ((buffer_at_offset(input_buffer)[0] == '-') || ((buffer_at_offset(input_buffer)[0] >= '0') && (buffer_at_offset(input_buffer)[0] <= '9')))) {
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

    return false;
}

typedef struct {
    const unsigned char *json;
    size_t position;
} error;
static error global_error = {NULL, 0};

PyObject *pycJSON_Decode(PyObject *self, PyObject *args, PyObject *kwargs) {
    parse_buffer buffer = {0, 0, 0, 0, {0, 0, 0}};
    PyObject **item = global_hooks.allocate(sizeof(PyObject *));
    cJSON_bool require_null_terminated = 0;
    const char **return_parse_end = 0;

    /* reset error position */
    global_error.json = NULL;
    global_error.position = 0;

    PyObject *arg = PyTuple_GET_ITEM(args, 0);
    const char *value = PyUnicode_AsUTF8(arg);
    size_t buffer_length = strlen(value);
    if (value == NULL || 0 == buffer_length) {
        goto fail;
    }
    buffer.content = (const unsigned char *) value;
    buffer.length = buffer_length;
    buffer.offset = 0;
    buffer.hooks = global_hooks;
    if (!parse_value(item, buffer_skip_whitespace(skip_utf8_bom(&buffer)))) {
        /* parse failure. ep is set. */
        goto fail;
    }

    /* if we require null-terminated JSON without appended garbage, skip and then check for a null terminator */
    if (require_null_terminated) {
        buffer_skip_whitespace(&buffer);
        if ((buffer.offset >= buffer.length) || buffer_at_offset(&buffer)[0] != '\0') {
            goto fail;
        }
    }
    if (return_parse_end) {
        *return_parse_end = (const char *) buffer_at_offset(&buffer);
    }

    PyObject *re = *item;
    global_hooks.deallocate(item);
    return re;

fail:
    if (item != NULL) {
        global_hooks.deallocate(item);
    }

    if (value != NULL) {
        error local_error;
        local_error.json = (const unsigned char *) value;
        local_error.position = 0;

        if (buffer.offset < buffer.length) {
            local_error.position = buffer.offset;
        } else if (buffer.length > 0) {
            local_error.position = buffer.length - 1;
        }

        if (return_parse_end != NULL) {
            *return_parse_end = (const char *) local_error.json + local_error.position;
        }

        global_error = local_error;
    }

    PyErr_SetString(PyExc_ValueError, "Failed to parse JSON");

    Py_RETURN_NONE;
}

PyObject *pycJSON_DecodeFile(PyObject *self, PyObject *args, PyObject *kwargs) {
    Py_RETURN_NOTIMPLEMENTED;
}