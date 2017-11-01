#include "json.h"

/*  */
static json_key_value null_key_value = { .key = NULL, .value = NULL, .type = json_type_nothing };

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
