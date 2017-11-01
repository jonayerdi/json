#include "json.h"

/*  */
static json_key_value null_key_value = { .key = NULL, .value = NULL, .type = json_type_nothing };

/* Parsing a json type from an input stream */

json_state json_read_string(json_allocator allocator, json_input_stream input, json_string *data_out)
{

}

json_state json_read_integer(json_allocator allocator, json_input_stream input, json_integer *data_out)
{

}

json_state json_read_decimal(json_allocator allocator, json_input_stream input, json_decimal *data_out)
{

}

json_state json_read_object(json_allocator allocator, json_input_stream input, json_object *data_out)
{

}

json_state json_read_array(json_allocator allocator, json_input_stream input, json_array *data_out)
{

}

/* Writing a json type to an output stream */

json_state json_write_string(json_style *style, json_output_stream output, json_string data_in)
{

}

json_state json_write_integer(json_style *style, json_output_stream output, json_integer data_in)
{

}

json_state json_write_decimal(json_style *style, json_output_stream output, json_decimal data_in)
{

}

json_state json_write_object(json_style *style, json_output_stream output, json_object data_in)
{

}

json_state json_write_array(json_style *style, json_output_stream output, json_array data_in)
{

}

/* Read and write functions for string streams */

size_t json_string_input_stream_read(void *data, size_t size, size_t count, void *args)
{
    size_t currentCount;
    json_string_stream_state *state = (json_string_stream_state *) args;
    for(currentCount = 0 ; currentCount < count ; currentCount++)
    {
        if(state->position + size <= state->size)
        {
             memcpy(data, &state->buffer[state->position], size);
             state->position += size;
        }
        else
            break;
    }
    return currentCount;
}

size_t json_string_output_stream_write(void *data, size_t size, size_t count, void *args)
{
    size_t currentCount;
    json_string_stream_state *state = (json_string_stream_state *) args;
    for(currentCount = 0 ; currentCount < count ; currentCount++)
    {
        if(state->position + size <= state->size)
        {
             memcpy(&state->buffer[state->position], data, size);
             state->position += size;
        }
        else
            break;
    }
    return currentCount;
}

/* Parse 4-digit hex json_string */
json_state json_parse_hex(json_char *input, size_t count, json_char *output)
{
    json_char tmp;
    for(size_t i = 0 ; i < count ; i++)
    {
        tmp = JSON_HEX_VALUE(input[i]);
        if(tmp == -1)
            return json_state_error_parse;
        if(i%2)
            output[i/2] = tmp;
        else
            output[i/2] += tmp << 4;
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
