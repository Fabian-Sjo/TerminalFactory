#include "ADT.h"
#include <stdio.h>
#include <stdlib.h>

DynamicArray *dynamicArrayCreate(int dataSize, int initialSize)
{

	DynamicArray *array = malloc(sizeof(DynamicArray));
	array->numberOfElements = 0;
	array->dataSize = dataSize;
	array->capacity = initialSize;
	array->elements = malloc(initialSize * dataSize);

	printf("Memory successfully allocated using "
		   "malloc.\n");
	return array;
}
void DynamicArrayDestroy(DynamicArray *array)
{
	free(array->elements);
	free(array);
}
int dynamicArrayAdd(DynamicArray *array, void *element)
{

	if (array->numberOfElements == array->capacity)
	{
		array->elements = realloc(array->elements, (array->numberOfElements + 32) * array->dataSize);
	}
	void *destination = (char *)array->elements + array->numberOfElements * array->dataSize;

	memcpy(destination, element, array->dataSize);
	array->numberOfElements++;
	return 0;
}

int dynamicArraySet(DynamicArray *array, int index, void *element)
{
	if (index >= array->numberOfElements || index < 0)
	{
		return -1;
	}
	void *destination = (char *)array->elements + index * array->dataSize;

	memcpy(destination, element, array->dataSize);
	return 0;
}

void *dynamicArrayGet(DynamicArray *array, int index)
{
	if (index >= array->numberOfElements || index < 0)
	{
		return NULL;
	}
	void *destination = (char *)array->elements + index * array->dataSize;

	return destination;
}

// Define the Node structure

struct Node
{
	struct Node *next;
};
LinkedList *linkedListCreate(int dataSize)
{
	LinkedList *list = (LinkedList *)malloc(sizeof(LinkedList));
	list->head = NULL;
	list->dataSize = dataSize;
	list->nrOfElements = 0;
	return list;
}
void linkedListDestroy(LinkedList *linkedList)
{
	struct Node *current = linkedList->head;
	struct Node *next;

	while (current != NULL)
	{
		next = current->next;
		free(current);
		current = next;
	}
	free(linkedList);
}
void *linkedListGet(LinkedList *linkedList, int index)
{
	if (index >= linkedList->nrOfElements || index < 0)
	{
		return NULL;
	}
	struct Node *temp = linkedList->head;
	for (int i = 0; i < index; i++)
	{
		temp = temp->next;
	}
	return (char *)temp + sizeof(struct Node);
}
// Function to create a new node
struct Node *linkedListcreateNode(LinkedList *linkedList, void *data)
{
	struct Node *newNode = (struct Node *)malloc(sizeof(struct Node) + linkedList->dataSize);
	memcpy((char *)newNode + sizeof(struct Node), data, linkedList->dataSize);
	newNode->next = NULL;
	return newNode;
}

// Function to insert a new element at the beginning of the singly linked list
void linkedListinsertAtFirst(LinkedList *linkedList, void *data)
{
	struct Node *newNode = linkedListcreateNode(linkedList, data);
	newNode->next = linkedList->head;
	linkedList->head = newNode;
	linkedList->nrOfElements++;
}

// Function to insert a new element at the end of the singly linked list
void linkedListinsertAtEnd(LinkedList *linkedList, void *data)
{
	struct Node *newNode = linkedListcreateNode(linkedList, data);
	if (linkedList->head == NULL)
	{
		linkedList->head = newNode;
		linkedList->nrOfElements++;
		return;
	}
	struct Node *temp = linkedList->head;
	while (temp->next != NULL)
	{
		temp = temp->next;
	}
	temp->next = newNode;
	linkedList->nrOfElements++;
	return;
}

// Function to insert a new element at a specific position in the singly linked list
void linkedListinsertAtPosition(LinkedList *linkedList, void *data, int position)
{
	struct Node *newNode = linkedListcreateNode(linkedList, data);
	if (position == 0)
	{
		return linkedListinsertAtFirst(linkedList, data);
	}
	struct Node *temp = linkedList->head;
	for (int i = 0; temp != NULL && i < position - 1; i++)
	{
		temp = temp->next;
	}
	if (temp == NULL)
	{
		free(newNode);
		return -1;
	}
	newNode->next = temp->next;
	temp->next = newNode;
	linkedList->nrOfElements++;
	return;
}

// Function to delete the first node of the singly linked list
int linkedListdeleteFromFirst(LinkedList *linkedList)
{
	if (linkedList->head == NULL)
	{
		return -1;
	}
	struct Node *temp = linkedList->head;
	linkedList->head = temp->next;
	linkedList->nrOfElements--;
	free(temp);
	return 0;
}

// Function to delete the last node of the singly linked list
int linkedListdeleteFromEnd(LinkedList *linkedList)
{
	if (linkedList->head == NULL)
	{
		return -1;
	}
	struct Node *temp = linkedList->head;
	if (temp->next == NULL)
	{
		free(temp);
		linkedList->head = NULL;
		linkedList->nrOfElements--;

		return 0;
	}
	while (temp->next->next != NULL)
	{
		temp = temp->next;
	}
	free(temp->next);
	temp->next = NULL;
	linkedList->nrOfElements--;
	return 0;
}

// Function to delete a node at a specific position in the singly linked list
int linkedListdeleteAtPosition(LinkedList *linkedList, int position)
{
	if (linkedList->head == NULL)
	{
		return -1;
	}
	struct Node *temp = linkedList->head;
	if (position == 0)
	{
		return linkedListdeleteFromFirst(linkedList);
	}
	for (int i = 0; temp != NULL && i < position - 1; i++)
	{
		temp = temp->next;
	}
	if (temp == NULL || temp->next == NULL)
	{
		printf("Position out of range\n");
		return -2;
	}
	struct Node *next = temp->next->next;
	free(temp->next);
	temp->next = next;
	linkedList->nrOfElements--;
	return 0;
}

// Function to print the LinkedList
void linkedListprint(LinkedList *linkedList)
{
	struct Node *temp = linkedList->head;
	for (size_t i = 0; i < linkedList->nrOfElements; i++)
	{
		printf("%d -> ", linkedListGet(linkedList, i));
	}

	printf("NULL\n");
}
