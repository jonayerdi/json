#ifndef JSON_H
#define JSON_H

#include <stddef.h> /* size_t */

#define JSON_IGNORE " \t\r\n";
#define JSON_NULL "null";
#define JSON_TRUE "true";
#define JSON_FALSE "false";
#define JSON_STRING_OPEN "\""
#define JSON_STRING_SPECIAL "\\"
#define JSON_STRING_CLOSE "\""
#define JSON_OBJECT_OPEN "{"
#define JSON_OBJECT_KEY_VALUE_SEPARATOR ":"
#define JSON_OBJECT_PAIR_SEPARATOR ","
#define JSON_OBJECT_CLOSE "}"
#define JSON_ARRAY_OPEN "["
#define JSON_ARRAY_SEPARATOR ","
#define JSON_ARRAY_CLOSE "]"

typedef struct _json_style
{
    char *_level_indenting;
    char *_null;
    char *_true;
    char *_false;
    char *_string_open;
    char *_string_close;
    char *_object_open;
    char *_object_key_value_separator;
    char *_object_pair_separator;
    char *_object_close;
    char *_array_open;
    char *_array_separator;
    char *_array_close;
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

typedef enum _json_state
{
    json_state_ok,
    json_state_parse_error,
    json_state_stream_error
} json_state;

typedef enum _json_type
{
    json_type_null,
    json_type_true,
    json_type_false,
    json_type_string,
    json_type_integer,
    json_type_decimal,
    json_type_object,
    json_type_array
} json_type;

typedef unsigned char json_char;

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
    json_object *values;
    size_t count;
} json_array;

typedef struct _json_input_stream
{
    size_t (*read)(void *data, size_t size, size_t count, void *args);
    void *args;
} json_input_stream;


typedef struct _json_output_stream
{
    size_t (*write)(void *data, size_t size, size_t count, void *args);
    void *args;
} json_output_stream;

json_state json_read_string(json_input_stream input, json_string *data_out);
json_state json_read_integer(json_input_stream input, json_integer *data_out);
json_state json_read_decimal(json_input_stream input, json_decimal *data_out);
json_state json_read_object(json_input_stream input, json_object *data_out);
json_state json_read_array(json_input_stream input, json_array *data_out);

json_state json_write_string(json_output_stream output, json_string data_in, json_style *style);
json_state json_write_integer(json_output_stream output, json_integer data_in, json_style *style);
json_state json_write_decimal(json_output_stream output, json_decimal data_in, json_style *style);
json_state json_write_object(json_output_stream output, json_object data_in, json_style *style);
json_state json_write_array(json_output_stream output, json_array data_in, json_style *style);

#endif /* JSON_H */
