#include "json.h"

#define json_output_string2(src, len) \
{ \
    if(len > (length - index)) \
        return json_state_error_buffer; \
    json_string_copy2(src, ((json) + (index)), len); \
    index += len; \
}

#define json_output_string(src) json_output_string2(src, json_string_length(src))

#define json_string_contains(str, ch) (memchr((str), ((int)ch), json_string_length(str)) == NULL)

#define json_indent(num) { for(json_integer i = 0 ; i < num ; i++) { json_output_string(style->_level_indenting); } }

/* Freeing json types */
void json_free_value(json_allocator *allocator, json_value value)
{
    switch(value.type)
    {
        //Container types
        case json_type_array:
            json_array *array = (json_array *)value.value;
            json_free_array(allocator, *array);
            allocator->free(array);
            break;
        case json_type_object:
            json_object *object = (json_object *)value.value;
            json_free_object(allocator, *object);
            allocator->free(object);
            break;
        //Simple types
        case json_type_string:
        case json_type_integer:
        case json_type_decimal:
            allocator->free(value.value);
            break;
    }
}
inline void json_free_object(json_allocator *allocator, json_object object)
{
    for(size_t i = 0 ; i < object.count ; i++)
    {
        json_free_value(allocator, object.values[i].value);
        allocator->free(object.values[i].key);
    }
    allocator->free(object.values);
}
inline void json_free_array(json_allocator *allocator, json_array array)
{
    for(size_t i = 0 ; i < array.count ; i++)
        json_free_value(allocator, array.values[i]);
    allocator->free(array.values);
}

json_state json_write_value(json_string json, size_t length, json_integer indent, json_style *style, json_value *data_in, size_t *written)
{
    size_t index = 0;
    size_t sub_written;
    json_state sub_retval;

    switch(data_in->type)
    {
        case json_type_null:
            json_indent(indent);
            json_output_string(style->_null);
            break;
        case json_type_true:
            json_indent(indent);
            json_output_string(style->_true);
            break;
        case json_type_false:
            json_indent(indent);
            json_output_string(style->_false);
            break;
        case json_type_decimal:
            sub_retval = json_write_decimal(json + index, length - index, indent, style, (json_decimal*)(data_in->value), &sub_written);
            if(sub_retval != json_state_ok)
                return sub_retval;
            index += sub_written;
            break;
        case json_type_integer:
            sub_retval = json_write_integer(json + index, length - index, indent, style, (json_integer*)(data_in->value), &sub_written);
            if(sub_retval != json_state_ok)
                return sub_retval;
            index += sub_written;
            break;
        case json_type_string:
            sub_retval = json_write_string(json + index, length - index, indent, style, (json_string)(data_in->value), &sub_written);
            if(sub_retval != json_state_ok)
                return sub_retval;
            index += sub_written;
            break;
        case json_type_array:
            sub_retval = json_write_array(json + index, length - index, indent, style, (json_array*)(data_in->value), &sub_written);
            if(sub_retval != json_state_ok)
                return sub_retval;
            index += sub_written;
            break;
        case json_type_object:
            sub_retval = json_write_object(json + index, length - index, indent, style, (json_object*)(data_in->value), &sub_written);
            if(sub_retval != json_state_ok)
                return sub_retval;
            index += sub_written;
            break;
    }

    *written = index;

    return json_state_ok;
}
json_state json_write_string(json_string json, size_t length, json_integer indent, json_style *style, json_string data_in, size_t *written)
{
    size_t index = 0;
    json_string str = data_in;
    size_t strlength = json_string_length(str);

    json_indent(indent);

    json_output_string(style->_string_open);

    for(size_t i = 0 ; i < strlength ; i++)
    {
        switch(str[i])
        {
            case '\"':
                json_output_string("\\\"");
                break;
            case '\\':
                json_output_string("\\\\");
                break;
            case '/':
                json_output_string("\\/");
                break;
            case '\b':
                json_output_string("\\b");
                break;
            case '\f':
                json_output_string("\\f");
                break;
            case '\n':
                json_output_string("\\n");
                break;
            case '\r':
                json_output_string("\\r");
                break;
            case '\t':
                json_output_string("\\t");
                break;
            default:
                json_output_string2(str + i, 1);
                break;
        }
    }

    json_output_string(style->_string_close);

    *written = index;

    return json_state_ok;
}
json_state json_write_integer(json_string json, size_t length, json_integer indent, json_style *style, json_integer *data_in, size_t *written)
{
    size_t index = 0;
    int size;
    char result[22];

    size = sprintf(result, "%lld", *data_in);
    if(size > 21)
        return json_state_error_buffer;

    json_indent(indent);

    json_output_string(result);

    *written = index;

    return json_state_ok;    
}
json_state json_write_decimal(json_string json, size_t length, json_integer indent, json_style *style, json_decimal *data_in, size_t *written)
{
    size_t index = 0;
    int size;
    char result[256];

    size = sprintf(result, "%.200lf", *data_in);
    if(size > 255)
        return json_state_error_buffer;
    
    json_indent(indent);

    json_output_string(result);

    *written = index;

    return json_state_ok; 
}
json_state json_write_key_value(json_string json, size_t length, json_integer indent, json_style *style, json_key_value *data_in, size_t *written)
{
    size_t index = 0;
    size_t sub_written;
    json_state sub_retval;

    json_indent(indent);

    sub_retval = json_write_string(json + index, length - index, indent, style, data_in->key, &sub_written);
    if(sub_retval != json_state_ok)
        return sub_retval;
    index += sub_written;

    json_output_string(style->_object_key_value_separator);

    sub_retval = json_write_value(json + index, length - index, indent, style, &data_in->value, &sub_written);
    if(sub_retval != json_state_ok)
        return sub_retval;
    index += sub_written;

    *written = index;

    return json_state_ok;
}
json_state json_write_object(json_string json, size_t length, json_integer indent, json_style *style, json_object *data_in, size_t *written)
{
    size_t index = 0;
    size_t sub_written;
    json_state sub_retval;

    json_indent(indent);

    json_output_string(style->_object_open);

    for(size_t i = 0 ; i < data_in->count ; i++)
    {
        sub_retval = json_write_key_value(json + index, length - index, indent + 1, style, &data_in->values[i], &sub_written);
        if(sub_retval != json_state_ok)
            return sub_retval;
        index += sub_written;

        if(i < data_in->count - 1)
            json_output_string(style->_object_pair_separator);
    }

    json_indent(indent);

    json_output_string(style->_object_close);

    *written = index;

    return json_state_ok;
}
json_state json_write_array(json_string json, size_t length, json_integer indent, json_style *style, json_array *data_in, size_t *written)
{
    size_t index = 0;
    size_t sub_written;
    json_state sub_retval;

    json_indent(indent);

    json_output_string(style->_array_open);

    for(size_t i = 0 ; i < data_in->count ; i++)
    {
        sub_retval = json_write_value(json + index, length - index, indent + 1, style, &data_in->values[i], &sub_written);
        if(sub_retval != json_state_ok)
            return sub_retval;
        index += sub_written;

        if(i < data_in->count - 1)
            json_output_string(style->_array_separator);
    }

    json_indent(indent);

    json_output_string(style->_array_close);

    *written = index;

    return json_state_ok;
}

/*  */
static json_key_value null_key_value = { .key = NULL, .value = NULL };

/* Parse 4-digit hex json_string */
json_state json_parse_hex(json_char *input, size_t count, json_char *output)
{
    json_char digit;
    for(size_t i = 0 ; i < count ; i++)
    {
        digit = JSON_HEX_VALUE(input[i]);
        if(digit == -1)
            return json_state_error_parse;
        if(i%2 == 0)
            output[i/2] = digit << 4;
        else
            output[i/2] += digit;
    }
    
    return json_state_ok;
}

/* Searching a json_object for a given key */
json_key_value json_object_find_key(json_object object, json_string key, size_t num)
{
    for(size_t i = 0 ; i < object.count ; i++)
        if(json_string_compare(key, object.values[i].key) == 0)
        {
            if(num > 0)
                num--;
            else
                return object.values[i];
        }
    return null_key_value;
}
