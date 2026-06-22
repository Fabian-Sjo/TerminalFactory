#include <stdio.h>
#include <assert.h>
#include "../src/utils/ADT.h"

static void verifyArray(
	DynamicArray *array,
	int *expected,
	int count)
{
	assert(array->numberOfElements == count);

	for (int i = 0; i < count; i++)
	{
		int *value = (int *)dynamicArrayGet(array, i);

		assert(value != NULL);
		assert(*value == expected[i]);
	}
}

void testDynamicArray()
{
	printf("=== DynamicArray tests ===\n");

	/*
	 * Create
	 */
	printf("Testing create...\n");

	DynamicArray *array = dynamicArrayCreate(sizeof(int), 2);

	assert(array != NULL);
	assert(array->dataSize == sizeof(int));
	assert(array->numberOfElements == 0);
	assert(array->capacity >= 2);

	/*
	 * Empty access
	 */
	printf("Testing empty access...\n");

	assert(dynamicArrayGet(array, 0) == NULL);
	assert(dynamicArrayGet(array, -1) == NULL);
	assert(dynamicArrayGet(array, 100) == NULL);

	/*
	 * Add
	 */
	printf("Testing add...\n");

	int a = 10;
	int b = 20;
	int c = 30;

	assert(dynamicArrayAdd(array, &a) == 0);
	assert(dynamicArrayAdd(array, &b) == 0);
	assert(dynamicArrayAdd(array, &c) == 0);

	int expected[] = {10, 20, 30};

	verifyArray(array, expected, 3);

	/*
	 * Resize
	 */
	printf("Testing resize...\n");

	for (int i = 3; i < 10000; i++)
	{
		int value = i;

		assert(dynamicArrayAdd(array, &value) == 0);
	}

	assert(array->numberOfElements == 10000);

	for (int i = 0; i < 10000; i++)
	{
		int *value = dynamicArrayGet(array, i);

		assert(value != NULL);

		if (i == 0)
			assert(*value == 10);
		else if (i == 1)
			assert(*value == 20);
		else if (i == 2)
			assert(*value == 30);
		else
			assert(*value == i);
	}

	/*
	 * Set
	 */
	printf("Testing set...\n");

	int newValue = 999;

	assert(dynamicArraySet(array, 0, &newValue) == 0);
	int returnedVal = (*(int *)dynamicArrayGet(array, 0));
	assert(returnedVal == 999);

	/*
	 * Invalid set
	 */
	printf("Testing invalid set...\n");

	assert(dynamicArraySet(array, -1, &newValue) != 0);
	assert(dynamicArraySet(array, 999999, &newValue) != 0);

	/*
	 * Different data sizes
	 */
	printf("Testing different data sizes...\n");

	DynamicArray *chars =
		dynamicArrayCreate(sizeof(char), 1);

	assert(chars);

	char x = 'A';
	char y = 'B';

	assert(dynamicArrayAdd(chars, &x) == 0);
	assert(dynamicArrayAdd(chars, &y) == 0);

	assert(*(char *)dynamicArrayGet(chars, 0) == 'A');
	assert(*(char *)dynamicArrayGet(chars, 1) == 'B');

	/*
	 * Destroy
	 */
	printf("Testing destroy...\n");

	assert(dynamicArrayDestroy(chars) == 0);
	assert(dynamicArrayDestroy(array) == 0);

	printf("All DynamicArray tests passed.\n");
}

int main()
{
	testDynamicArray();

	return 0;
}