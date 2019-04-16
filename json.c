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

#include "json.h"

/* Parsing utils */
static int json_is_whitespace(json_char character)
{
    for(json_size i = 0 ; i < json_string_length(JSON_WHITESPACE_CHARS) ; i++)
    {
        if(character == JSON_WHITESPACE_CHARS[i])
        {
            return 1;
        }
    }
    return 0;
}
static json_char json_next_token(const json_char *json, json_size length, json_size *read)
{
    json_size index = 0;
    do
    {
        if(index == length)
        {
            *read = 0;
            return 0;
        }
    } while(json_is_whitespace(json[index++]));
    *read = index;
    return json[index-1];
}

/* Freeing json types */
void json_free_value(const json_value *value)
{
    switch(value->type)
    {
        // Container types
        case json_type_array:
            json_free_array(&value->value_array);
            break;
        case json_type_object:
            json_free_object(&value->value_obj);
            break;
        case json_type_string:
            json_free(value->value_str);
            break;
        // Simple types
        default:
        	break;
    }
}
void json_free_object(const json_object *object)
{
    for(json_size i = 0 ; i < object->count ; i++)
    {
        json_free_value(&object->values[i].value);
        json_free(object->values[i].key);
    }
    json_free(object->values);
}
void json_free_array(const json_array *array)
{
    for(json_size i = 0 ; i < array->count ; i++)
        json_free_value(&array->values[i]);
    json_free(array->values);
}

/* Read json elements */
json_state json_read_value(const json_char *json, json_size length, json_value *data_out, json_size *read)
{
    json_size index = 0;
    json_state sub_retval;
    json_size sub_read;
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
            sub_retval = json_read_string(json + index, length - index, &data_out->value_str, &sub_read);
            if(sub_retval != json_state_ok)
                return sub_retval;
            *read = index + sub_read;
            break;
        case '{':
            data_out->type = json_type_object;
            sub_retval = json_read_object(json + index, length - index, &data_out->value_obj, &sub_read);
            if(sub_retval != json_state_ok)
            {
                return sub_retval;
            }
            *read = index + sub_read;
            break;
        case '[':
            data_out->type = json_type_array;
            sub_retval = json_read_array(json + index, length - index, &data_out->value_array, &sub_read);
            if(sub_retval != json_state_ok)
            {
                return sub_retval;
            }
            *read = index + sub_read;
            break;
        default:
            //Number
            sub_retval = json_read_integer(json + index, length - index, &data_out->value_int, &sub_read);
            if(sub_retval != json_state_ok)
            {
                return sub_retval;
            }
            if(json[index + sub_read] == JSON_DECIMAL_COMMA[0]) //Decimal
            {
                data_out->type = json_type_decimal;
                sub_retval = json_read_decimal(json + index, length - index, &data_out->value_dec, &sub_read);
                if(sub_retval != json_state_ok)
                {
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
json_state json_read_string(const json_char *json, json_size length, json_char **data_out, json_size *read)
{
    json_size index = 0;
    json_size chars = 20;
    json_size out_index = 0;
    json_size sub_read;
    json_char *buffer;
    char escaped = 0;

    if(json_next_token(json + index, length - index, &sub_read) != JSON_STRING_OPEN[0])
    {
        return json_state_error_parse;
    }
    index += sub_read;

    buffer = json_malloc(sizeof(json_char) * chars);
    if(buffer == NULL)
    {
        return json_state_error_malloc;
    }

    while(index < length)
    {
        if(out_index >= chars)
        {
            chars *= 2;
            buffer = json_realloc(buffer, chars);
            if(buffer == NULL)
            {
                return json_state_error_malloc;
            }
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
                    json_free(buffer);
                    return json_state_error_parse;
            }
        }
        else
        {
            switch(json[index])
            {
                case '\"':
                    buffer = json_realloc(buffer, sizeof(json_char) * (out_index + 1));
                    if(buffer == NULL)
                    {
                        return json_state_error_malloc;
                    }
                    buffer[out_index] = '\0';
                    *data_out = buffer;
                    *read = index + 1;
                    return json_state_ok;
                case '\\':
                    escaped = 1;
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
json_state json_read_integer(const json_char *json, json_size length, json_integer *data_out, json_size *read)
{
    json_size index = 0;
    int digit;
    json_integer result = 0;
    char negative = 0;
    json_size sub_read;

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
json_state json_read_decimal(const json_char *json, json_size length, json_decimal *data_out, json_size *read)
{
    json_size index = 0;
    json_size decimals = 0;
    int digit;
    json_decimal result = 0.0;
    char negative = 0;
    json_size sub_read;

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

    for(json_size i = 0 ; i < (decimals - 1) ; i++)
        result /= 10.0;
    *data_out = negative ? -result : result;
    *read = index;

    return json_state_ok;
}
json_state json_read_key_value(const json_char *json, json_size length, json_key_value *data_out, json_size *read)
{
    json_size index = 0;
    json_size sub_read;
    json_state sub_retval;

    sub_retval = json_read_string(json + index, length - index, &data_out->key, &sub_read);
    if(sub_retval != json_state_ok)
        return sub_retval;
    index += sub_read;

    if(json_next_token(json + index, length - index, &sub_read) != JSON_OBJECT_KEY_VALUE_SEPARATOR[0])
    {
        json_free(data_out->key);
        return json_state_error_parse;
    }
    index += sub_read;

    sub_retval = json_read_value(json + index, length - index, &data_out->value, &sub_read);
    if(sub_retval != json_state_ok)
    {
        json_free(data_out->key);
        return sub_retval;
    }
    index += sub_read;

    *read = index;

    return json_state_ok;
}
json_state json_read_object(const json_char *json, json_size length, json_object *data_out, json_size *read)
{
    json_size index = 0;
    json_size sub_read;
    json_state sub_retval;
    json_char next_token;
    json_key_value *buffer;
    json_size element_count = 0;

    if(json_next_token(json + index, length - index, &sub_read) != JSON_OBJECT_OPEN[0])
    {
        return json_state_error_parse;
    }
    index += sub_read;

    buffer = json_malloc(sizeof(json_key_value));
    if(buffer == NULL)
    {
        return json_state_error_malloc;
    }

    while(json_next_token(json + index, length - index, &sub_read) != JSON_OBJECT_CLOSE[0])
    {
        sub_retval = json_read_key_value(json + index, length - index, buffer + element_count, &sub_read);
        if(sub_retval != json_state_ok)
        {
            for(json_size i = 0 ; i < element_count ; i++)
            {
                json_free(buffer[i].key);
                json_free_value(&buffer[i].value);
            }
            json_free(buffer);
            return sub_retval;
        }
        index += sub_read;
        element_count++;

        next_token = json_next_token(json + index, length - index, &sub_read);
        if(next_token == JSON_OBJECT_PAIR_SEPARATOR[0])
        {
            index += sub_read;
            buffer = json_realloc(buffer, sizeof(json_key_value) * (element_count + 1));
            if(buffer == NULL)
            {
                return json_state_error_malloc;
            }
        }
        else if(next_token != JSON_OBJECT_CLOSE[0])
        {
            for(json_size i = 0 ; i < element_count ; i++)
            {
                json_free(buffer[i].key);
                json_free_value(&buffer[i].value);
            }
            json_free(buffer);
            return json_state_error_parse;
        }
    }
    index += sub_read;

    data_out->count = element_count;
    data_out->values = buffer;
    *read = index;

    return json_state_ok;
}
json_state json_read_array(const json_char *json, json_size length, json_array *data_out, json_size *read)
{
    json_size index = 0;
    json_size sub_read;
    json_state sub_retval;
    json_char next_token;
    json_value *buffer;
    json_size element_count = 0;

    if(json_next_token(json + index, length - index, &sub_read) != JSON_ARRAY_OPEN[0])
    {
        return json_state_error_parse;
    }
    index += sub_read;

    buffer = json_malloc(sizeof(json_value));
    if(buffer == NULL)
    {
        return json_state_error_malloc;
    }

    while(json_next_token(json + index, length - index, &sub_read) != JSON_ARRAY_CLOSE[0])
    {
        sub_retval = json_read_value(json + index, length - index, buffer + element_count, &sub_read);
        if(sub_retval != json_state_ok)
        {
            for(json_size i = 0 ; i < element_count ; i++)
            {
                json_free_value(&buffer[i]);
            }
            json_free(buffer);
            return sub_retval;
        }
        index += sub_read;
        element_count++;

        next_token = json_next_token(json + index, length - index, &sub_read);
        if(next_token == JSON_ARRAY_SEPARATOR[0])
        {
            index += sub_read;
            buffer = json_realloc(buffer, sizeof(json_value) * (element_count + 1));
            if(buffer == NULL)
            {
                return json_state_error_malloc;
            }
        }
        else if(next_token != JSON_ARRAY_CLOSE[0])
        {
            for(json_size i = 0 ; i < element_count ; i++)
            {
                json_free_value(&buffer[i]);
            }
            json_free(buffer);
            return json_state_error_parse;
        }
    }
    index += sub_read;

    data_out->count = element_count;
    data_out->values = buffer;
    *read = index;

    return json_state_ok;
}

/* Searching a json_object for a given key */
json_value *json_object_find_key(const json_object *object, const json_char *key, json_size num)
{
    json_key_value *keyvalue = object->values;
    for(json_size i = 0 ; i < object->count ; i++)
    {
        if(json_string_compare(key, keyvalue->key) == 0)
        {
            if(num > 0)
                num--;
            else
                return &keyvalue->value;
        }
        keyvalue++;
    }
    return NULL;
}
