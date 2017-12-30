#ifndef JSON_H
#define JSON_H

#include <stddef.h> /* size_t */
#include <stdio.h> /* sprintf */
#include <string.h> /* strlen, strcmp, memcpy, memchr  */

/* Hex digits */
#define _JSON_IF_IN_RANGE_ELSE(C, RF, RL, I, E) ((((C) >= (RF)) && ((C) <= (RL))) ? (I) : (E))
#define JSON_DECIMAL_VALUE(CHAR) _JSON_IF_IN_RANGE_ELSE(CHAR, '0', '9', CHAR - '0', -1)
#define JSON_HEX_VALUE(CHAR) _JSON_IF_IN_RANGE_ELSE(CHAR, '0', '9', CHAR - '0', _JSON_IF_IN_RANGE_ELSE(CHAR, 'A', 'F', CHAR - 'A' + 10, _JSON_IF_IN_RANGE_ELSE(CHAR, 'a', 'f', CHAR - 'a' + 10, -1)))

/* json_string length */
#define json_string_length(str) strlen(str)
/* json_string compare */
#define json_string_compare(str1, str2) strcmp(str1, str2)
/* json_string copy */
#define json_string_copy2(src, dst, len) memcpy(dst, src, len * sizeof(json_char))
#define json_string_copy(src, dst) json_string_copy2(dst, src, json_string_length(src))

/* Special json characters or strings */
#define JSON_IGNORE " \t\r\n"
#define JSON_NULL "null"
#define JSON_TRUE "true"
#define JSON_FALSE "false"
#define JSON_STRING_OPEN "\""
#define JSON_STRING_CLOSE "\""
#define JSON_DECIMAL_COMMA "."
#define JSON_OBJECT_OPEN "{"
#define JSON_OBJECT_KEY_VALUE_SEPARATOR ":"
#define JSON_OBJECT_PAIR_SEPARATOR ","
#define JSON_OBJECT_CLOSE "}"
#define JSON_ARRAY_OPEN "["
#define JSON_ARRAY_SEPARATOR ","
#define JSON_ARRAY_CLOSE "]"

/* Definition of the json_char */
typedef char json_char;

/* Return type for json_* */
typedef enum _json_state
{
    json_state_ok =                 0,
    json_state_error_malloc =       1<<0,
    json_state_error_buffer =       1<<1,
    json_state_error_parse =        1<<2,
    json_state_error_encoding =     1<<3,
    json_state_error_type =         1<<4
} json_state;

/* json value types */
typedef enum _json_type
{
    json_type_nothing,
    json_type_null,
    json_type_true,
    json_type_false,
    json_type_string,
    json_type_integer,
    json_type_decimal,
    json_type_object,
    json_type_array
} json_type;

/* Generic json value */
typedef struct _json_value
{
    void *value;
    json_type type;
} json_value;

typedef json_char *json_string;

typedef signed long long json_integer;

typedef double json_decimal;

typedef struct _json_key_value
{
    json_string key;
    json_value value;
} json_key_value;

typedef struct _json_object
{
    json_key_value *values;
    size_t count;
} json_object;

typedef struct _json_array
{
    json_value *values;
    size_t count;
} json_array;

/* json styles for json_write_* */
typedef struct _json_style
{
    json_string _level_indenting;
    json_string _null;
    json_string _true;
    json_string _false;
    json_string _string_open;
    json_string _string_close;
    json_string _object_open;
    json_string _object_key_value_separator;
    json_string _object_pair_separator;
    json_string _object_close;
    json_string _array_open;
    json_string _array_separator;
    json_string _array_close;
} json_style;

#define JSON_STYLE_COMPACT \
{ \
    ._level_indenting = "", \
    ._null = JSON_NULL, \
    ._true = JSON_TRUE, \
    ._false = JSON_FALSE, \
    ._string_open = JSON_STRING_OPEN, \
    ._string_close = JSON_STRING_CLOSE, \
    ._object_open = JSON_OBJECT_OPEN, \
    ._object_key_value_separator = JSON_OBJECT_KEY_VALUE_SEPARATOR, \
    ._object_pair_separator = JSON_OBJECT_PAIR_SEPARATOR, \
    ._object_close = JSON_OBJECT_CLOSE, \
    ._array_open = JSON_ARRAY_OPEN, \
    ._array_separator = JSON_ARRAY_SEPARATOR, \
    ._array_close = JSON_ARRAY_CLOSE \
}

#define JSON_STYLE_TABS \
{ \
    ._level_indenting = "\t", \
    ._null = JSON_NULL, \
    ._true = JSON_TRUE, \
    ._false = JSON_FALSE, \
    ._string_open = JSON_STRING_OPEN, \
    ._string_close = JSON_STRING_CLOSE, \
    ._object_open = "{\n", \
    ._object_key_value_separator = ": ", \
    ._object_pair_separator = ",\n", \
    ._object_close = "\n}", \
    ._array_open = "[\n", \
    ._array_separator = ",\n", \
    ._array_close = "\n]" \
}

#define JSON_STYLE_4SPACES \
{ \
    ._level_indenting = "    ", \
    ._null = JSON_NULL, \
    ._true = JSON_TRUE, \
    ._false = JSON_FALSE, \
    ._string_open = JSON_STRING_OPEN, \
    ._string_close = JSON_STRING_CLOSE, \
    ._object_open = "{\n", \
    ._object_key_value_separator = ": ", \
    ._object_pair_separator = ",\n", \
    ._object_close = "\n}", \
    ._array_open = "[\n", \
    ._array_separator = ",\n", \
    ._array_close = "\n]" \
}

static json_style json_style_compact = JSON_STYLE_COMPACT;
static json_style json_style_tabs = JSON_STYLE_TABS;
static json_style json_style_4spaces = JSON_STYLE_4SPACES;

/* Memory allocator for json_read_* */
typedef struct _json_allocator
{
    void *(*malloc)(size_t size);
    void (*free)(void *ptr);
} json_allocator;

#define JSON_STD_ALLOCATOR { .malloc = malloc, .free = free }

/* Freeing json values */
void json_free_value(json_allocator *allocator, json_value value);
void json_free_object(json_allocator *allocator, json_object value);
void json_free_array(json_allocator *allocator, json_array value);

/* Parsing a json type from an unicode string */
json_state json_read_value(json_string json, size_t length, json_allocator *allocator, json_value *data_out, size_t *read);
json_state json_read_string(json_string json, size_t length, json_allocator *allocator, json_string *data_out, size_t *read);
json_state json_read_integer(json_string json, size_t length, json_allocator *allocator, json_integer *data_out, size_t *read);
json_state json_read_decimal(json_string json, size_t length, json_allocator *allocator, json_decimal *data_out, size_t *read);
json_state json_read_key_value(json_string json, size_t length, json_allocator *allocator, json_key_value *data_out, size_t *read);
json_state json_read_object(json_string json, size_t length, json_allocator *allocator, json_object *data_out, size_t *read);
json_state json_read_array(json_string json, size_t length, json_allocator *allocator, json_array *data_out, size_t *read);

/* Writing a json type to an unicode string */
json_state json_write_value(json_string json, size_t length, json_integer indent, json_style *style, json_value *data_in, size_t *written);
json_state json_write_string(json_string json, size_t length, json_integer indent, json_style *style, json_string data_in, size_t *written);
json_state json_write_integer(json_string json, size_t length, json_integer indent, json_style *style, json_integer *data_in, size_t *written);
json_state json_write_decimal(json_string json, size_t length, json_integer indent, json_style *style, json_decimal *data_in, size_t *written);
json_state json_write_key_value(json_string json, size_t length, json_integer indent, json_style *style, json_key_value *data_in, size_t *written);
json_state json_write_object(json_string json, size_t length, json_integer indent, json_style *style, json_object *data_in, size_t *written);
json_state json_write_array(json_string json, size_t length, json_integer indent, json_style *style, json_array *data_in, size_t *written);

/* Parse 4-digit hex json_string */
json_state json_parse_hex(json_char *input, size_t count, json_char *output);

/* Searching a json_object for a given key */
json_value json_object_find_key(json_object object, json_string key, size_t num);

#endif /* JSON_H */
