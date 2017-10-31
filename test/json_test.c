#include "json_test.h"

/**************************************************************** MEMORY LEAKS ***********************************************************************/
void *malloc_test(const size_t size)
{
	void *ptr = malloc(size);
	test_assert(memory_allocations_count + 1 <= MAX_MEMORY_ALLOCATIONS); //Increase MAX_MEMORY_ALLOCATIONS if needed
	memory_allocations[memory_allocations_count++] = ptr; //Store allocated memory pointer
	return ptr;
}

void free_test(void *ptr)
{
	int found = 0;
	for(size_t i = 0 ; i < memory_allocations_count ; i++)
	{
		if(memory_allocations[i] == ptr)
		{
			ARRAY_SHIFT_LEFT(memory_allocations, i, memory_allocations_count); //Remove freed element from allocations list
			memory_allocations_count--;
			found = 1; 
			break;
		}
	}
	test_assert(found);
	free(ptr);
}
/**************************************************************** MEMORY LEAKS ***********************************************************************/

int main(int argc, char *argv[])
{
    JSON_HEX_VALUE_test();
}

/******************************************************************** TESTS **************************************************************************/
int JSON_HEX_VALUE_test(void)
{

}
/******************************************************************** TESTS **************************************************************************/
