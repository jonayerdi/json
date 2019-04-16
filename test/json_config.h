#ifndef JSON_CONFIG_H
#define JSON_CONFIG_H

#include <stdlib.h> /* size_t, malloc, free, realloc */
#include <stdio.h> /* sprintf */
#include <string.h> /* strlen, strcmp, memcpy */

#include "json_test.h"

typedef size_t json_size;

/* json string utils */
#define json_string_length(str) strlen(str)
#define json_string_compare(str1, str2) strcmp(str1, str2)
#define json_string_copy2(src, dst, len) memcpy(dst, src, len * sizeof(json_char))
#define json_string_copy(src, dst) json_string_copy2(dst, src, json_string_length(src))

/* memory allocation */
#define json_malloc(size) malloc_test(size)
#define json_free(ptr) free_test(ptr)
#define json_realloc(ptr, size) realloc_test(ptr, size)

#endif /* JSON_CONFIG_H */
