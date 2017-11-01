#ifndef JSON_H
#define JSON_H

#include <stddef.h> /* size_t */
#include <string.h> /* memcpy, strlen, strcmp */

/* Hex digits */
#define _JSON_IF_IN_RANGE_ELSE(C, RF, RL, I, E) ((((C) >= (RF)) && ((C) <= (RL))) ? (I) : (E))
#define JSON_HEX_VALUE(CHAR) _JSON_IF_IN_RANGE_ELSE(CHAR, '0', '9', CHAR - '0', _JSON_IF_IN_RANGE_ELSE(CHAR, 'A', 'F', CHAR - 'A' + 10, _JSON_IF_IN_RANGE_ELSE(CHAR, 'a', 'f', CHAR - 'a' + 10, -1)))

/* json_string length */
#define json_string_length(str) strlen(str)
/* json_string compare */
#define json_string_compare(str1, str2) strcmp(str1, str2)

/* Special json characters or strings */
#define JSON_IGNORE " \t\r\n"
#define JSON_NULL "null"
#define JSON_TRUE "true"
#define JSON_FALSE "false"
#define JSON_STRING_OPEN "\""
#define JSON_STRING_CLOSE "\""
#define JSON_OBJECT_OPEN "{"
#define JSON_OBJECT_KEY_VALUE_SEPARATOR ":"
#define JSON_OBJECT_PAIR_SEPARATOR ","
#define JSON_OBJECT_CLOSE "}"
#define JSON_ARRAY_OPEN "["
#define JSON_ARRAY_SEPARATOR ","
#define JSON_ARRAY_CLOSE "]"
#define JSON_STRING_SPECIAL \
{ \
    { "\"", "\\\"" }, \
    { "\\", "\\\\" }, \
    { "\/", "\\/" }, \
    { "\b", "\\b" }, \
    { "\f", "\\f" }, \
    { "\n", "\\n" }, \
    { "\t", "\\t" }, \
    { "\\u", "\\\\u" }  \
}

/* ENcodings */
typedef enum _json_encoding
{
    json_utf8,
    json_utf16,
    json_utf32
} json_encoding;

/* Definition of the json_char */
typedef char json_char;

/* Return type for json_read_* and json_write_* */
typedef enum _json_state
{
    json_state_ok =                 0,
    json_state_error_malloc =       1<<0,
    json_state_error_buffer =       1<<1,
    json_state_error_parse =        1<<2,
    json_state_error_encoding =     1<<3
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

typedef json_char *json_string;

typedef signed long long json_integer;

typedef double json_decimal;

typedef struct _json_key_value
{
    json_string key;
    void *value;
    json_type type;
} json_key_value;

typedef struct _json_object
{
    json_key_value *values;
    size_t count;
} json_object;

typedef struct _json_array
{
    void **values;
    json_type *types;
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
    ._object_open = JSON_OBJECT_OPEN, \
    ._object_key_value_separator = " : ", \
    ._object_pair_separator = ",\n", \
    ._object_close = JSON_OBJECT_CLOSE, \
    ._array_open = JSON_ARRAY_OPEN, \
    ._array_separator = ",\n", \
    ._array_close = JSON_ARRAY_CLOSE \
}

#define JSON_STYLE_4SPACES \
{ \
    ._level_indenting = "    ", \
    ._null = JSON_NULL, \
    ._true = JSON_TRUE, \
    ._false = JSON_FALSE, \
    ._string_open = JSON_STRING_OPEN, \
    ._string_close = JSON_STRING_CLOSE, \
    ._object_open = JSON_OBJECT_OPEN, \
    ._object_key_value_separator = " : ", \
    ._object_pair_separator = ",\n", \
    ._object_close = JSON_OBJECT_CLOSE, \
    ._array_open = JSON_ARRAY_OPEN, \
    ._array_separator = ",\n", \
    ._array_close = JSON_ARRAY_CLOSE \
}

/* Memory allocator for json_read_* */
typedef struct _json_allocator
{
    void *(*malloc)(size_t size);
    void (*free)(void);
} json_allocator;

#define JSON_STD_ALLOCATOR { .malloc = malloc, .free = free }

/* Encoding */
json_state json_utf8_to_utf16(json_string in, size_t max_read, json_string out, size_t max_write);
json_state json_utf8_to_utf32(json_string in, size_t max_read, json_string out, size_t max_write);
json_state json_utf16_to_utf8(json_string in, size_t max_read, json_string out, size_t max_write);
json_state json_utf32_to_utf8(json_string in, size_t max_read, json_string out, size_t max_write);

/* Parsing a json type from an unicode string */
json_state json_read_string(json_string json, size_t length, json_encoding encoding, json_allocator allocator, json_string *data_out);
json_state json_read_integer(json_string json, size_t length, json_encoding encoding, json_allocator allocator, json_integer *data_out);
json_state json_read_decimal(json_string json, size_t length, json_encoding encoding, json_allocator allocator, json_decimal *data_out);
json_state json_read_key_value(json_string json, size_t length, json_encoding encoding, json_allocator allocator, json_key_value *data_out);
json_state json_read_object(json_string json, size_t length, json_encoding encoding, json_allocator allocator, json_object *data_out);
json_state json_read_array(json_string json, size_t length, json_encoding encoding, json_allocator allocator, json_array *data_out);

/* Writing a json type to an unicode string */
json_state json_write_string(json_string json, size_t length, json_encoding encoding, json_style style, json_string data_in);
json_state json_write_integer(json_string json, size_t length, json_encoding encoding, json_style style, json_integer data_in);
json_state json_write_decimal(json_string json, size_t length, json_encoding encoding, json_style style, json_decimal data_in);
json_state json_write_key_value(json_string json, size_t length, json_encoding encoding, json_style style, json_key_value data_in);
json_state json_write_object(json_string json, size_t length, json_encoding encoding, json_style style, json_object data_in);
json_state json_write_array(json_string json, size_t length, json_encoding encoding, json_style style, json_array data_in);

/* Parse 4-digit hex json_string */
json_state json_parse_hex(json_char *input, size_t count, json_char *output);

/* Searching a json_object for a given key */
json_key_value json_object_find_key(json_object object, json_string key, size_t num);

#endif /* JSON_H */
