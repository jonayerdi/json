#ifndef JSON_H
#define JSON_H

#include <stddef.h> /* size_t */
#include <string.h> /* memcpy, strlen, strcmp */

/* Hex digits */
#define _JSON_IF_IN_RANGE_ELSE(C, RF, RL, I, E) ((((C) >= _J_C(RF)) && ((C) <= _J_C(RL))) ? (I) : (E))
#define JSON_HEX_VALUE(CHAR) _JSON_IF_IN_RANGE_ELSE(CHAR, '0', '9', CHAR - '0', _JSON_IF_IN_RANGE_ELSE(CHAR, 'A', 'F', CHAR - 'A' + 10, _JSON_IF_IN_RANGE_ELSE(CHAR, 'a', 'f', CHAR - 'a' + 10, -1)))

/* json_char conversion */
#define _J_C(CHAR) ((json_char) (CHAR))
/* json_string conversion */
#define _J_S(STR) ((json_string) (STR))
/* json_string length */
#define json_string_length(str) strlen(str)
/* json_string compare */
#define json_string_compare(str1, str2) strcmp(str1, str2)

/* Special json characters or strings */
#define JSON_IGNORE _J_S(" \t\r\n")
#define JSON_NULL _J_S("null")
#define JSON_TRUE _J_S("true")
#define JSON_FALSE _J_S("false")
#define JSON_STRING_OPEN _J_S("\"")
#define JSON_STRING_CLOSE _J_S("\"")
#define JSON_OBJECT_OPEN _J_S("{")
#define JSON_OBJECT_KEY_VALUE_SEPARATOR _J_S(":")
#define JSON_OBJECT_PAIR_SEPARATOR _J_S(",")
#define JSON_OBJECT_CLOSE _J_S("}")
#define JSON_ARRAY_OPEN _J_S("[")
#define JSON_ARRAY_SEPARATOR _J_S(",")
#define JSON_ARRAY_CLOSE _J_S("]")
#define JSON_STRING_SPECIAL \
{ \
    { _J_S("\""), _J_S("\\\"") }, \
    { _J_S("\\"), _J_S("\\\\") }, \
    { _J_S("\/"), _J_S("\\/") }, \
    { _J_S("\b"), _J_S("\\b") }, \
    { _J_S("\f"), _J_S("\\f") }, \
    { _J_S("\n"), _J_S("\\n") }, \
    { _J_S("\t"), _J_S("\\t") }, \
    { _J_S("\\u"), _J_S("\\\\u") }  \
}

/* Enum of supported charsets */
typedef enum _json_charset
{
    json_utf8,
    json_utf16,
    json_utf32
} json_charset;

/* Definition of the json_char */
typedef char json_char;

/* Input stream for json_read_* */
typedef struct _json_input_stream
{
    size_t (*read)(void *data, size_t size, size_t count, void *args);
    void *args;
} json_input_stream;

/* Output stream for json_write_* */
typedef struct _json_output_stream
{
    size_t (*write)(void *data, size_t size, size_t count, void *args);
    void *args;
} json_output_stream;

/* Read and write to/from streams*/
#define json_stream_read(stream, data, size, count) ((stream).read(data, size*sizeof(json_char), count*sizeof(json_char), (stream).args))
#define json_stream_write(stream, data, size, count) ((stream).write(data, size*sizeof(json_char), count*sizeof(json_char), (stream).args))

/* Structure to keep track of string stream state */
typedef struct _json_string_stream_state
{
    json_char *buffer;
    size_t size;
    size_t position;
} json_string_stream_state;

/* File and string streams */
#define json_file_input_stream(FILEPTR) { read = fread, args = (FILEPTR) }
#define json_file_output_stream(FILEPTR) { write = fwrite, args = (FILEPTR) }
#define json_string_input_stream(STRING) { read = json_string_input_stream_read, args = { buffer = (STRING), size = json_string_length(STRING), position = 0 } }
#define json_string_output_stream(BUFFER, BUFSIZE) { write = json_string_output_stream_write, args = { buffer = (BUFFER), size = (BUFSIZE), position = 0 } }

/* Return type for json_read_* and json_write_* */
typedef enum _json_state
{
    json_state_ok,
    json_state_parse_error,
    json_state_stream_error
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
    json_object *values;
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
    ._level_indenting = _J_S(""), \
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
    ._level_indenting = _J_S("\t"), \
    ._null = JSON_NULL, \
    ._true = JSON_TRUE, \
    ._false = JSON_FALSE, \
    ._string_open = JSON_STRING_OPEN, \
    ._string_close = JSON_STRING_CLOSE, \
    ._object_open = JSON_OBJECT_OPEN, \
    ._object_key_value_separator = _J_S(" : "), \
    ._object_pair_separator = _J_S(",\n"), \
    ._object_close = JSON_OBJECT_CLOSE, \
    ._array_open = JSON_ARRAY_OPEN, \
    ._array_separator = _J_S(",\n"), \
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

/* json_reader */
typedef struct _json_reader
{
    json_charset charset;
    json_allocator allocator;
    json_input_stream in;
} json_reader;

/* json_reader */
typedef struct _json_writer
{
    json_charset charset;
    json_style style;
    json_output_stream out;
} json_writer;

/* Parsing a json type from an input stream */
json_state json_read_string(json_reader *reader, json_string *data_out);
json_state json_read_integer(json_reader *reader, json_integer *data_out);
json_state json_read_decimal(json_reader *reader, json_decimal *data_out);
json_state json_read_object(json_reader *reader, json_object *data_out);
json_state json_read_array(json_reader *reader, json_array *data_out);

/* Writing a json type to an output stream */
json_state json_write_string(json_writer *writer, json_string data_in);
json_state json_write_integer(json_writer *writer, json_integer data_in);
json_state json_write_decimal(json_writer *writer, json_decimal data_in);
json_state json_write_object(json_writer *writer, json_object data_in);
json_state json_write_array(json_writer *writer, json_array data_in);

/* Read and write functions for string streams */
size_t json_string_input_stream_read(void *data, size_t size, size_t count, void *args);
size_t json_string_output_stream_write(void *data, size_t size, size_t count, void *args);

/* Parse 4-digit hex json_string */
json_state json_parse_hex(json_reader *reader, json_char output[2]);

/* Searching a json_object for a given key */
json_key_value json_find_key(json_object object, json_string key);

#endif /* JSON_H */
