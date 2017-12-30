/***********************************************************************************
zlib License

Copyright (c) 2017 Jon Ayerdi

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
            json_free_array(allocator, *(json_array *)value.value);
            allocator->free(value.value);
            break;
        case json_type_object:
            json_free_object(allocator, *(json_object *)value.value);
            allocator->free(value.value);
            break;
        //Simple types
        case json_type_string:
        case json_type_integer:
        case json_type_decimal:
            allocator->free(value.value);
            break;
        default:
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
inline int json_can_ignore(json_char character)
{
    for(size_t i = 0 ; i < json_string_length(JSON_IGNORE) ; i++)
        if(character == JSON_IGNORE[i])
            return 1;
    return 0;
}
inline json_char json_next_token(json_string json, size_t length, size_t *read)
{
    size_t index = 0;
    do
    {
        if(index == length)
        {
            *read = 0;
            return 0;
        }
    } while(json_can_ignore(json[index++]));
    *read = index;
    return json[index-1];
}
json_state json_read_value(json_string json, size_t length, json_allocator *allocator, json_value *data_out, size_t *read)
{
    size_t index = 0;
    json_state sub_retval;
    size_t sub_read;
    json_char next_token;

    next_token = json_next_token(json + index, length - index, &sub_read);
    index = sub_read - 1;

    switch(next_token)
    {
        case 'n':
            data_out->type = json_type_null;
            *read = index + json_string_length(JSON_NULL);
            break;
        case 't':
            data_out->type = json_type_true;
            *read = index + json_string_length(JSON_TRUE);
            break;
        case 'f':
        data_out->type = json_type_false;
            *read = index + json_string_length(JSON_FALSE);
            break;
        case '\"':
            data_out->type = json_type_string;
            sub_retval = json_read_string(json + index, length - index, allocator, (json_string *)&data_out->value, &sub_read);
            if(sub_retval != json_state_ok)
                return sub_retval;
            *read = index + sub_read;
            break;
        case '{':
            data_out->type = json_type_object;
            data_out->value = allocator->malloc(sizeof(json_object));
            if(data_out->value == NULL)
                return json_state_error_malloc;
            sub_retval = json_read_object(json + index, length - index, allocator, (json_object *)data_out->value, &sub_read);
            if(sub_retval != json_state_ok)
            {
                allocator->free(data_out->value);
                return sub_retval;
            }
            *read = index + sub_read;
            break;
        case '[':
            data_out->type = json_type_array;
            data_out->value = allocator->malloc(sizeof(json_array));
            if(data_out->value == NULL)
                return json_state_error_malloc;
            sub_retval = json_read_array(json + index, length - index, allocator, (json_array *)data_out->value, &sub_read);
            if(sub_retval != json_state_ok)
            {
                allocator->free(data_out->value);
                return sub_retval;
            }
            *read = index + sub_read;
            break;
        default:
            //Number
            data_out->value = allocator->malloc(sizeof(json_integer));
            if(data_out->value == NULL)
                return json_state_error_malloc;
            sub_retval = json_read_integer(json + index, length - index, allocator, (json_integer *)data_out->value, &sub_read);
            if(sub_retval != json_state_ok)
            {
                allocator->free(data_out->value);
                return sub_retval;
            }
            if(json[index + sub_read] == JSON_DECIMAL_COMMA[0]) //Decimal
            {
                data_out->type = json_type_decimal;
                allocator->free(data_out->value);
                data_out->value = allocator->malloc(sizeof(json_decimal));
                if(data_out->value == NULL)
                    return json_state_error_malloc;
                sub_retval = json_read_decimal(json + index, length - index, allocator, (json_decimal *)data_out->value, &sub_read);
                if(sub_retval != json_state_ok)
                {
                    allocator->free(data_out->value);
                    return sub_retval;
                }
            }
            else
                data_out->type = json_type_integer;
            *read = index + sub_read;
            break;
    }

    return json_state_ok;
}
json_state json_read_string(json_string json, size_t length, json_allocator *allocator, json_string *data_out, size_t *read)
{
    size_t index = 0;
    size_t out_index = 0;
    size_t sub_read;
    json_string buffer, buffer2;
    char escaped = 0;

    if(json_next_token(json + index, length - index, &sub_read) != JSON_STRING_OPEN[0])
        return json_state_error_parse;
    index += sub_read;

    buffer = (json_string)allocator->malloc(sizeof(json_char));
    if(buffer == NULL)
        return json_state_error_malloc;

    while(index < length)
    {
        if(out_index % 20 == 0)
        {
            buffer2 = (json_string)allocator->malloc(sizeof(json_char) * (out_index + 20));
            if(buffer2 == NULL)
            {
                allocator->free(buffer);
                return json_state_error_malloc;
            }
            memcpy(buffer2, buffer, out_index);
            allocator->free(buffer);
            buffer = buffer2;
        }
        if(escaped)
        {
            escaped = 0;
            switch(json[index])
            {
                case '\"':
                    buffer[out_index++] = '\"';
                    break;
                case '\\':
                    buffer[out_index++] = '\\';
                    break;
                case '/':
                    buffer[out_index++] = '/';
                    break;
                case 'b':
                    buffer[out_index++] = '\b';
                    break;
                case 'f':
                    buffer[out_index++] = '\f';
                    break;
                case 'n':
                    buffer[out_index++] = '\n';
                    break;
                case 'r':
                    buffer[out_index++] = '\r';
                    break;
                case 't':
                    buffer[out_index++] = '\t';
                    break;
                case 'u':
                    //TODO
                    break;
                default:
                    allocator->free(buffer);
                    return json_state_error_parse;
            }
        }
        else
        {
            switch(json[index])
            {
                case '\"':
                    buffer[out_index++] = '\0';
                    buffer2 = (json_string)allocator->malloc(sizeof(json_char) * (out_index + 20));
                    if(buffer2 == NULL)
                    {
                        allocator->free(buffer);
                        return json_state_error_malloc;
                    }
                    memcpy(buffer2, buffer, out_index);
                    allocator->free(buffer);
                    *data_out = buffer2;
                    *read = index + 1;
                    return json_state_ok;
                case '\\':
                    escaped = 1;
                    break;
                case '\0':

                    break;
                default:
                    buffer[out_index++] = json[index];
                    break;
            }
        }
        index++;
    }

    return json_state_error_buffer;
}
json_state json_read_integer(json_string json, size_t length, json_allocator *allocator, json_integer *data_out, size_t *read)
{
    size_t index = 0;
    int digit;
    json_integer result = 0;
    char negative = 0;
    size_t sub_read;

    json_next_token(json + index, length - index, &sub_read);
    index = sub_read - 1;

    if(index < length && json[index] == '-')
    {
        negative = 1;
        index++;
    }

    while(index < length)
    {
        digit = JSON_DECIMAL_VALUE(json[index]);
        if(digit < 0)
            break;
        result = (result * 10) + digit;
        index++;
    }

    *data_out = negative ? -result : result;
    *read = index;

    return json_state_ok;
}
json_state json_read_decimal(json_string json, size_t length, json_allocator *allocator, json_decimal *data_out, size_t *read)
{
    size_t index = 0;
    size_t decimals = 0;
    int digit;
    json_decimal result = 0.0;
    char negative = 0;
    size_t sub_read;

    json_next_token(json + index, length - index, &sub_read);
    index = sub_read - 1;

    if(index < length && json[index] == '-')
    {
        negative = 1;
        index++;
    }

    while(index < length)
    {
        digit = JSON_DECIMAL_VALUE(json[index]);
        if(digit < 0)
        {
            if(!decimals && json[index] == JSON_DECIMAL_COMMA[0])
                decimals = 1;
            else
                break;
        }
        else
        {
            result = (result * 10) + (json_decimal)digit;
            if(decimals)
                decimals++;
        }
        index++;
    }

    for(size_t i = 0 ; i < (decimals - 1) ; i++)
        result /= 10.0;
    *data_out = negative ? -result : result;
    *read = index;

    return json_state_ok;
}
json_state json_read_key_value(json_string json, size_t length, json_allocator *allocator, json_key_value *data_out, size_t *read)
{
    size_t index = 0;
    size_t sub_read;
    json_state sub_retval;

    sub_retval = json_read_string(json + index, length - index, allocator, &data_out->key, &sub_read);
    if(sub_retval != json_state_ok)
        return sub_retval;
    index += sub_read;

    if(json_next_token(json + index, length - index, &sub_read) != JSON_OBJECT_KEY_VALUE_SEPARATOR[0])
    {
        allocator->free(data_out->key);
        return json_state_error_parse;
    }
    index += sub_read;

    sub_retval = json_read_value(json + index, length - index, allocator, &data_out->value, &sub_read);
    if(sub_retval != json_state_ok)
    {
        allocator->free(data_out->key);
        return sub_retval;
    }
    index += sub_read;

    *read = index;

    return json_state_ok;
}
json_state json_read_object(json_string json, size_t length, json_allocator *allocator, json_object *data_out, size_t *read)
{
    size_t index = 0;
    size_t sub_read;
    json_state sub_retval;
    json_char next_token;
    json_key_value *buffer, *buffer2 = NULL;
    size_t element_count = 0;

    if(json_next_token(json + index, length - index, &sub_read) != JSON_OBJECT_OPEN[0])
        return json_state_error_parse;
    index += sub_read;

    buffer = (json_key_value *)allocator->malloc(sizeof(json_key_value));
    if(buffer == NULL)
        return json_state_error_malloc;

    while(json_next_token(json + index, length - index, &sub_read) != JSON_OBJECT_CLOSE[0])
    {
        sub_retval = json_read_key_value(json + index, length - index, allocator, buffer + element_count, &sub_read);
        if(sub_retval != json_state_ok)
        {
            for(size_t i = 0 ; i < element_count ; i++)
            {
                allocator->free(buffer[i].key);
                json_free_value(allocator, buffer[i].value);
            }
            allocator->free(buffer);
            return sub_retval;
        }
        index += sub_read;
        element_count++;

        next_token = json_next_token(json + index, length - index, &sub_read);
        if(next_token == JSON_OBJECT_PAIR_SEPARATOR[0])
        {
            index += sub_read;
            buffer2 = (json_key_value *)allocator->malloc(sizeof(json_key_value) * (element_count + 1));
            if(buffer2 == NULL)
            {
                allocator->free(buffer);
                return json_state_error_malloc;
            }
            memcpy(buffer2, buffer, sizeof(json_key_value) * element_count);
            allocator->free(buffer);
            buffer = buffer2;
        }
        else if(next_token != JSON_OBJECT_CLOSE[0])
        {
            for(size_t i = 0 ; i < element_count ; i++)
            {
                allocator->free(buffer[i].key);
                json_free_value(allocator, buffer[i].value);
            }
            allocator->free(buffer);
            return json_state_error_parse;
        }
    }
    index += sub_read;

    data_out->count = element_count;
    data_out->values = buffer;
    *read = index;

    return json_state_ok;
}
json_state json_read_array(json_string json, size_t length, json_allocator *allocator, json_array *data_out, size_t *read)
{
    size_t index = 0;
    size_t sub_read;
    json_state sub_retval;
    json_char next_token;
    json_value *buffer, *buffer2 = NULL;
    size_t element_count = 0;

    if(json_next_token(json + index, length - index, &sub_read) != JSON_ARRAY_OPEN[0])
        return json_state_error_parse;
    index += sub_read;

    buffer = (json_value *)allocator->malloc(sizeof(json_value));
    if(buffer == NULL)
        return json_state_error_malloc;

    while(json_next_token(json + index, length - index, &sub_read) != JSON_ARRAY_CLOSE[0])
    {
        sub_retval = json_read_value(json + index, length - index, allocator, buffer + element_count, &sub_read);
        if(sub_retval != json_state_ok)
        {
            for(size_t i = 0 ; i < element_count ; i++)
                json_free_value(allocator, buffer[i]);
            allocator->free(buffer);
            return sub_retval;
        }
        index += sub_read;
        element_count++;

        next_token = json_next_token(json + index, length - index, &sub_read);
        if(next_token == JSON_ARRAY_SEPARATOR[0])
        {
            index += sub_read;
            buffer2 = (json_value *)allocator->malloc(sizeof(json_value) * (element_count + 1));
            if(buffer2 == NULL)
            {
                allocator->free(buffer);
                return json_state_error_malloc;
            }
            memcpy(buffer2, buffer, sizeof(json_value) * element_count);
            allocator->free(buffer);
            buffer = buffer2;
        }
        else if(next_token != JSON_ARRAY_CLOSE[0])
        {
            for(size_t i = 0 ; i < element_count ; i++)
                json_free_value(allocator, buffer[i]);
            allocator->free(buffer);
            return json_state_error_parse;
        }
    }
    index += sub_read;

    data_out->count = element_count;
    data_out->values = buffer;
    *read = index;

    return json_state_ok;
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
static json_value null_value = { .value = NULL, .type = json_type_nothing };

json_value json_object_find_key(json_object object, json_string key, size_t num)
{
    for(size_t i = 0 ; i < object.count ; i++)
        if(json_string_compare(key, object.values[i].key) == 0)
        {
            if(num > 0)
                num--;
            else
                return object.values[i].value;
        }
    return null_value;
}
