#include "json_test.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
	json_parse_hex_test();
	json_write_test1();
	test_assert(memory_allocations_count == 0); /* Check for memory leaks */
}

/******************************************************************** TESTS **************************************************************************/
int JSON_HEX_VALUE_test(void)
{
	/* Valid hex digits */
	test_assert(JSON_HEX_VALUE('0') == 0);
	test_assert(JSON_HEX_VALUE('1') == 1);
	test_assert(JSON_HEX_VALUE('2') == 2);
	test_assert(JSON_HEX_VALUE('3') == 3);
	test_assert(JSON_HEX_VALUE('4') == 4);
	test_assert(JSON_HEX_VALUE('5') == 5);
	test_assert(JSON_HEX_VALUE('6') == 6);
	test_assert(JSON_HEX_VALUE('7') == 7);
	test_assert(JSON_HEX_VALUE('8') == 8);
	test_assert(JSON_HEX_VALUE('9') == 9);
	test_assert(JSON_HEX_VALUE('a') == 10);
	test_assert(JSON_HEX_VALUE('b') == 11);
	test_assert(JSON_HEX_VALUE('c') == 12);
	test_assert(JSON_HEX_VALUE('d') == 13);
	test_assert(JSON_HEX_VALUE('e') == 14);
	test_assert(JSON_HEX_VALUE('f') == 15);
	test_assert(JSON_HEX_VALUE('A') == 10);
	test_assert(JSON_HEX_VALUE('B') == 11);
	test_assert(JSON_HEX_VALUE('C') == 12);
	test_assert(JSON_HEX_VALUE('D') == 13);
	test_assert(JSON_HEX_VALUE('E') == 14);
	test_assert(JSON_HEX_VALUE('F') == 15);
	/* Invalid hex digits */
	test_assert(JSON_HEX_VALUE('G') == -1);
	test_assert(JSON_HEX_VALUE('g') == -1);
	test_assert(JSON_HEX_VALUE('<') == -1);
	test_assert(JSON_HEX_VALUE('\n') == -1);
	test_assert(JSON_HEX_VALUE('\0') == -1);
	test_assert(memory_allocations_count == 0); /* Check for memory leaks */
	return 0;
}
int json_parse_hex_test(void)
{
	/* return value from  */
	/* Inputs and expected results */
	json_char *inputs[] = {"0000", "FFFF", "1234", "4321", "CAFE", "BEeF", "F1F4", "a34E"};
	json_char results[8][2] = {{0x00, 0x00}, {0xFF, 0xFF},{0x12, 0x34},{0x43, 0x21},{0xCA, 0xFE},{0xBE, 0xEF},{0xF1, 0xF4},{0xA3, 0x4E}};
	/* Check valid results */
	for(int i = 0 ; i < 8 ; i++)
	{
		json_char buffer[2];
		json_state result = json_parse_hex(inputs[i], strlen(inputs[i]), buffer);
		test_assert(result == json_state_ok);
		for(unsigned int digit = 0 ; digit < strlen(inputs[i])/2 ; digit++)
			test_assert(buffer[digit] == results[i][digit]);
	}
	/* Check invalid results */

	return 0;
}
int json_write_test1(void)
{
	json_state retval;
	json_char result[100];
	size_t written;
	json_allocator allocator = { .malloc = malloc_test, .free = free_test };
	json_object *root = (json_object *)malloc_test(sizeof(json_object));
	root->count = 2;
	json_key_value *rootValues = (json_key_value *)malloc_test(sizeof(json_key_value) * 2);
	root->values = rootValues;
	rootValues[0].key = (json_char *)malloc_test(sizeof(json_char) * 5);
	memcpy(rootValues[0].key, "key0", 5);
	rootValues[0].value.type = json_type_true;
	rootValues[1].key = (json_char *)malloc_test(sizeof(json_char) * 5);
	memcpy(rootValues[1].key, "key1", 5);
	rootValues[1].value.type = json_type_integer;
	rootValues[1].value.value = (json_integer *)malloc_test(sizeof(json_integer));
	*((json_integer *)rootValues[1].value.value) = 5;
	retval = json_write_object(result, 100, 0, &json_style_4spaces, root, &written);
	test_assert(retval == json_state_ok);
	result[written] = '\0';
	json_free_object(&allocator, *root);
	free_test(root);
	printf("%s", result);
	test_assert(memory_allocations_count == 0); /* Check for memory leaks */
	return 0;
}
/******************************************************************** TESTS **************************************************************************/
