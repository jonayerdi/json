#include "json_test.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**************************************************************** MEMORY LEAKS ***********************************************************************/
static void *memory_allocations[MAX_MEMORY_ALLOCATIONS];
static size_t memory_allocations_count = 0u;

void *malloc_test(size_t size)
{
	void *ptr = malloc(size);
	test_assert(memory_allocations_count + 1 <= MAX_MEMORY_ALLOCATIONS);
	memory_allocations[memory_allocations_count++] = ptr;
	return ptr;
}

void free_test(void *ptr)
{
	int found = 0;
	for(size_t i = 0 ; i < memory_allocations_count ; i++)
	{
		if(memory_allocations[i] == ptr)
		{
			ARRAY_SHIFT_LEFT(memory_allocations, i, memory_allocations_count);
			memory_allocations_count--;
			found = 1; 
			break;
		}
	}
	test_assert(found);
	free(ptr);
}

void *realloc_test(void *ptr, size_t size)
{
	int found = 0;
	for(size_t i = 0 ; i < memory_allocations_count ; i++)
	{
		if(memory_allocations[i] == ptr)
		{
			ARRAY_SHIFT_LEFT(memory_allocations, i, memory_allocations_count);
			memory_allocations_count--;
			found = 1; 
			break;
		}
	}
	test_assert(found);
	void *newptr = realloc(ptr, size);
	test_assert(memory_allocations_count + 1 <= MAX_MEMORY_ALLOCATIONS);
	memory_allocations[memory_allocations_count++] = newptr;
	return newptr;
}
/**************************************************************** MEMORY LEAKS ***********************************************************************/

int main(void)
{
	json_read_test();
	test_assert(memory_allocations_count == 0);
}

/******************************************************************** TESTS **************************************************************************/
int json_read_test(void)
{
	json_object root;
	json_char buffer[4096];
	json_size read;
	json_state retval;
	// Open file
	FILE *file = fopen("test/test.json", "rb");
	test_assert(file != NULL);
	// Read file and parse
	retval = json_read_object(buffer, fread(buffer, 1, 4096, file), &root, &read);
	test_assert(retval == json_state_ok);
	// Check object
	test_assert(root.count == 1);
	json_value *nodes = json_object_find_key(&root, "nodes", 0);
	test_assert(nodes != NULL);
	test_assert(nodes->type == json_type_array);
	json_array array = nodes->value_array;
	test_assert(array.count == 16);
	for(json_size i = 0 ; i < array.count ; i++)
	{
		test_assert(array.values[i].type == json_type_object);
		json_object node = array.values[i].value_obj;
		json_value *id = json_object_find_key(&node, "id", 0);
		test_assert(id != NULL);
		test_assert(id->type == json_type_integer);
		test_assert(id->value_int == (json_integer)i+1);
		json_value *paths = json_object_find_key(&node, "paths", 0);
		test_assert(paths != NULL);
		test_assert(paths->type == json_type_array);
		json_array paths_array = paths->value_array;
		test_assert(paths_array.count == 4);
		for(json_size j = 0 ; j < paths_array.count ; j++)
		{
			test_assert(paths_array.values[j].type == json_type_integer);
			printf("%lld,", paths_array.values[j].value_int);
		}
		printf("\t");
		json_value *position = json_object_find_key(&node, "position", 0);
		test_assert(position != NULL);
		test_assert(position->type == json_type_array);
		json_array position_array = position->value_array;
		test_assert(position_array.count == 2);
		for(json_size j = 0 ; j < position_array.count ; j++)
		{
			test_assert(position_array.values[j].type == json_type_integer);
			printf("%lld,", position_array.values[j].value_int);
		}
		printf("\n");
	}
	// Cleanup and check memory leaks
	json_free_object(&root);
	test_assert(memory_allocations_count == 0);
	return 0;
}
/******************************************************************** TESTS **************************************************************************/
