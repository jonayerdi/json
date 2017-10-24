#ifndef JSON_STREAM_HELPER_H
#define JSON_STREAM_HELPER_H

#include "json.h"
#include <string.h> /* memcpy, strlen */

#define json_file_input_stream(FILEPTR) { read = fread, args = (FILEPTR) }
#define json_file_output_stream(FILEPTR) { write = fwrite, args = (FILEPTR) }
#define json_string_input_stream(STRING) { read = json_string_input_stream_read, args = { buffer = (STRING), size = strlen(STRING), position = 0 } }
#define json_string_output_stream(BUFFER, BUFSIZE) { write = json_string_output_stream_write, args = { buffer = (BUFFER), size = (BUFSIZE), position = 0 } }

typedef struct _json_string_stream_state
{
    json_char *buffer;
    size_t size;
    size_t position;
} json_string_stream_state;

size_t json_string_input_stream_read(void *data, size_t size, size_t count, void *args);
size_t json_string_output_stream_write(void *data, size_t size, size_t count, void *args);

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

#endif /* JSON_STREAM_HELPER_H */
