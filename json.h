/***********************************************************************************
zlib License

Copyright (c) 2017-2019 Jon Ayerdi

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
***********************************************************************************/

#ifndef JSON_H
#define JSON_H

#include "json_config.h"

/* json_char to represented number */
#define JSON_DECIMAL_VALUE(CHAR) ((((CHAR) >= '0') && ((CHAR) <= '9')) ? ((CHAR) - '0') : (-1))

/* Special json characters or strings */
#define JSON_WHITESPACE_CHARS " \t\r\n"
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

/* json value types */
typedef enum _json_type
{
    json_type_nothing,      /* No value provided */
    json_type_null,         /* No value provided */
    json_type_true,         /* No value provided */
    json_type_false,        /* No value provided */
    json_type_string,
    json_type_integer,
    json_type_decimal,
    json_type_object,
    json_type_array
} json_type;

/* json char type */
typedef char json_char;

/* json signed integer type */
typedef signed long long json_integer;

/* json decimal type */
typedef double json_decimal;

/* json generic value and key-value pairs, structs defined below */
typedef struct _json_value json_value;
typedef struct _json_key_value json_key_value;

/* json key-value collection */
typedef struct _json_object
{
    json_key_value *values;
    json_size count;
} json_object;

/* json value collection */
typedef struct _json_array
{
    json_value *values;
    json_size count;
} json_array;

/* json generic value */
struct _json_value
{
    json_type type;
    union {
        json_char *value_str;
        json_integer value_int;
        json_decimal value_dec;
        json_object value_obj;
        json_array value_array;
    };
};

/* json object key-value pair */
struct _json_key_value
{
    json_char *key;
    json_value value;
};

/* Return type for json_* */
typedef enum _json_state
{
    json_state_ok =                 0,          /* OK */
    json_state_error_malloc =       1<<0,       /* malloc returned NULL */
    json_state_error_buffer =       1<<1,       /* json string prematurely ended */
    json_state_error_parse =        1<<2,       /* invalid json string */
} json_state;

/* Freeing json values */
void json_free_value(const json_value *value);
void json_free_object(const json_object *value);
void json_free_array(const json_array *value);

/* Parsing a json type from a string */
json_state json_read_value(const json_char *json, json_size length, json_value *data_out, json_size *read);
json_state json_read_string(const json_char *json, json_size length, json_char **data_out, json_size *read);
json_state json_read_integer(const json_char *json, json_size length, json_integer *data_out, json_size *read);
json_state json_read_decimal(const json_char *json, json_size length, json_decimal *data_out, json_size *read);
json_state json_read_key_value(const json_char *json, json_size length, json_key_value *data_out, json_size *read);
json_state json_read_object(const json_char *json, json_size length, json_object *data_out, json_size *read);
json_state json_read_array(const json_char *json, json_size length, json_array *data_out, json_size *read);

/* Searching a json_object for a given key */
json_value *json_object_find_key(const json_object *object, const json_char *key, json_size num);

#endif /* JSON_H */
