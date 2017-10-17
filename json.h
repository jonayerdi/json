#ifndef JSON_H
#define JSON_H

#include <stddef.h> /* size_t */

typedef enum _json_error
{
    json_error_ok,
    json_error_parse_failed,
    json_error_stream_failed
} json_error;

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

json_error json_read_string(json_input_stream input, json_string *data_out);
json_error json_read_integer(json_input_stream input, json_integer *data_out);
json_error json_read_decimal(json_input_stream input, json_decimal *data_out);
json_error json_read_object(json_input_stream input, json_object *data_out);
json_error json_read_array(json_input_stream input, json_array *data_out);

json_error json_write_string(json_output_stream output, json_string data_in);
json_error json_write_integer(json_output_stream output, json_integer data_in);
json_error json_write_decimal(json_output_stream output, json_decimal data_in);
json_error json_write_object(json_output_stream output, json_object data_in);
json_error json_write_array(json_output_stream output, json_array data_in);

#endif /* JSON_H */
