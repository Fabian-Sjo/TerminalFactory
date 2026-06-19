#ifndef ADT_H
#define ADT_H

typedef struct DynamicArray DynamicArray;
struct DynamicArray
{
	int capacity;
	int numberOfElements;
	int dataSize;
	void *elements;
};

DynamicArray *dynamicArrayCreate(int dataSize, int initialSize);

int dynamicArrayAdd(DynamicArray *array, void *element);

int dynamicArraySet(DynamicArray *array, int index, void *element);

void *dynamicArrayGet(DynamicArray *array, int index);

typedef struct LinkedList LinkedList;
struct LinkedList
{
	struct Node *head;
	int dataSize;
	int nrOfElements;
};

LinkedList *linkedListCreate(int dataSize);
void linkedListDestroy(LinkedList *linkedList);
void *linkedListGet(LinkedList *linkedList, int index);
// Function to create a new node
struct Node *linkedListcreateNode(LinkedList *linkedList, void *data);
// Function to insert a new element at the end of the singly linked list
void linkedListinsertAtEnd(LinkedList *linkedList, void *data);

// Function to insert a new element at a specific position in the singly linked list
void linkedListinsertAtPosition(LinkedList *linkedList, void *data, int position);

// Function to delete the first node of the singly linked list
int linkedListdeleteFromFirst(LinkedList *linkedList);
// Function to delete the last node of the singly linked list
int linkedListdeleteFromEnd(LinkedList *linkedList);

// Function to delete a node at a specific position in the singly linked list
int linkedListdeleteAtPosition(LinkedList *linkedList, int position);

// Function to print the LinkedList
void linkedListprint(LinkedList *linkedList);
#endif