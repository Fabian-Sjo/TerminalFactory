#include <stdio.h>
#include <assert.h>
#include "../src/utils/ADT.h"

static void verifyList(
	LinkedList *list,
	int *expected,
	int count)
{
	assert(list->nrOfElements == count);

	for (int i = 0; i < count; i++)
	{
		int *value = linkedListGet(list, i);

		assert(value != NULL);
		assert(*value == expected[i]);
	}

	assert(linkedListGet(list, count) == NULL);
}

int main(void)
{
	printf("Creating list...\n");

	LinkedList *list = linkedListCreate(sizeof(int));
	assert(list);

	/*
	 * Empty list tests
	 */
	printf("Testing empty list...\n");

	assert(list->nrOfElements == 0);
	assert(linkedListGet(list, 0) == NULL);
	assert(linkedListGet(list, -1) == NULL);

	assert(linkedListdeleteFromFirst(list) != 0);
	assert(linkedListdeleteFromEnd(list) != 0);
	assert(linkedListdeleteAtPosition(list, 0) != 0);

	/*
	 * Single element tests
	 */
	printf("Testing single element operations...\n");

	int a = 123;

	linkedListinsertAtEnd(list, &a);

	int expected1[] = {123};
	verifyList(list, expected1, 1);

	linkedListdeleteFromEnd(list);

	assert(list->nrOfElements == 0);
	assert(linkedListGet(list, 0) == NULL);

	linkedListinsertAtEnd(list, &a);

	verifyList(list, expected1, 1);

	linkedListdeleteFromFirst(list);

	assert(list->nrOfElements == 0);

	/*
	 * Insert at beginning
	 */
	printf("Testing insert at beginning...\n");

	int b = 10;
	int c = 20;
	int d = 30;

	linkedListinsertAtEnd(list, &b);
	linkedListinsertAtEnd(list, &d);

	linkedListinsertAtPosition(list, &c, 1);

	int expected2[] = {10, 20, 30};
	verifyList(list, expected2, 3);

	/*
	 * Insert at head repeatedly
	 */
	printf("Testing repeated head inserts...\n");

	int e = 5;

	linkedListinsertAtPosition(list, &e, 0);

	int expected3[] = {5, 10, 20, 30};
	verifyList(list, expected3, 4);

	/*
	 * Delete middle
	 */
	printf("Testing middle deletion...\n");

	linkedListdeleteAtPosition(list, 2);

	int expected4[] = {5, 10, 30};
	verifyList(list, expected4, 3);

	/*
	 * Delete head
	 */
	printf("Testing head deletion...\n");

	linkedListdeleteAtPosition(list, 0);

	int expected5[] = {10, 30};
	verifyList(list, expected5, 2);

	/*
	 * Delete tail
	 */
	printf("Testing tail deletion...\n");

	linkedListdeleteAtPosition(list, 1);

	int expected6[] = {10};
	verifyList(list, expected6, 1);

	/*
	 * Invalid positions
	 */
	printf("Testing invalid indices...\n");

	assert(linkedListGet(list, -100) == NULL);
	assert(linkedListGet(list, 99999) == NULL);

	assert(linkedListdeleteAtPosition(list, -1) != 0);
	assert(linkedListdeleteAtPosition(list, 99999) != 0);

	/*
	 * Large insertion test
	 */
	printf("Testing large insertion...\n");

	linkedListDestroy(list);
	list = linkedListCreate(sizeof(int));

#define COUNT 1000
	int values[COUNT];

	for (int i = 0; i < COUNT; i++)
	{
		values[i] = i;
		linkedListinsertAtEnd(list, &values[i]);
	}

	assert(list->nrOfElements == COUNT);

	for (int i = 0; i < COUNT; i++)
	{
		int *value = linkedListGet(list, i);

		assert(value);
		assert(*value == i);
	}

	/*
	 * Large deletion from front
	 */
	printf("Testing large front deletions...\n");

	for (int i = 0; i < COUNT; i++)
	{
		assert(list->nrOfElements == COUNT - i);

		int *value = linkedListGet(list, 0);

		assert(value);
		assert(*value == i);

		linkedListdeleteFromFirst(list);
	}

	assert(list->nrOfElements == 0);

	/*
	 * Large deletion from end
	 */
	printf("Testing large tail deletions...\n");

	for (int i = 0; i < COUNT; i++)
	{
		values[i] = i;
		linkedListinsertAtEnd(list, &values[i]);
	}

	for (int i = COUNT - 1; i >= 0; i--)
	{
		int *value = linkedListGet(list, i);

		assert(value);
		assert(*value == i);

		linkedListdeleteFromEnd(list);
	}

	assert(list->nrOfElements == 0);

	/*
	 * Alternating operations
	 */
	printf("Testing alternating insert/delete...\n");

	for (int i = 0; i < 500; i++)
	{
		linkedListinsertAtEnd(list, &i);

		int *value = linkedListGet(list, 0);
		assert(value);

		linkedListdeleteFromFirst(list);

		assert(list->nrOfElements == 0);
	}

	/*
	 * Stress test
	 */
	printf("Stress test...\n");

	for (int i = 0; i < 10000; i++)
	{
		int value = i;

		linkedListinsertAtEnd(list, &value);

		assert(list->nrOfElements > 0);

		linkedListdeleteFromEnd(list);

		assert(list->nrOfElements == 0);
	}
	/*
	 * To array
	 */
	printf("testing to array...\n");
	for (int i = 0; i < 100; i++)
	{
		linkedListinsertAtEnd(list, &i);
	}

	int *array = linkedListToArray(list);
	for (int i = 0; i < list->nrOfElements; i++)
	{
		int *value = linkedListGet(list, i);

		assert(value);
		assert(*value == array[i]);
	}

	linkedListDestroy(list);

	printf("ALL TESTS PASSED\n");

	return 0;
}