#ifndef JSON__TEST_H
#define JSON__TEST_H

#include "json.h"

#define test_assert(expression) \
if (!(expression)) \
{ \
	fprintf(stderr, "Assert failed: %s\nat function %s\nat line %d\n", #expression, __func__, __LINE__); \
	abort(); \
}

/* Shifts ARRAY to the left, changes values from ARRAY[INDEX] to ARRAY[SIZE-2] */
#define ARRAY_SHIFT_LEFT(ARRAY,INDEX,SIZE) for(size_t _index = (INDEX) ; _index < (SIZE)  - 1 ; _index++) { (ARRAY)[_index] = (ARRAY)[_index + 1]; }

/* Memory leak checking */
#define MAX_MEMORY_ALLOCATIONS 1024u
void *malloc_test(size_t size);
void free_test(void *ptr);
void *realloc_test(void *ptr, size_t size);

/* Tests */
int json_read_test(void);

#endif /* JSON__TEST_H */
