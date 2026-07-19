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

// Creates a dynamic array that stores elements of the given size and starts with the provided capacity.
DynamicArray *dynamicArrayCreate(int dataSize, int initialSize);
// Destroys the dynamic array and releases its backing storage.
int dynamicArrayDestroy(DynamicArray *array);

// Appends an element to the end of the array. The array grows automatically when needed and returns 0 on success.
int dynamicArrayAdd(DynamicArray *array, void *element);

// Replaces the element at index with the provided value. Out-of-range indices return -1.
int dynamicArraySet(DynamicArray *array, int index, void *element);

// Returns the element stored at the requested index, or null if the index is out of range.
void *dynamicArrayGet(DynamicArray *array, int index);

typedef struct LinkedList LinkedList;
struct LinkedList
{
	struct Node *head;
	int dataSize;
	int nrOfElements;
};

// Creates an empty linked list that stores elements of the given size.
LinkedList *linkedListCreate(int dataSize);
// Destroys the linked list and frees all associated nodes.
void linkedListDestroy(LinkedList *linkedList);
// Returns the element at the requested zero-based index, or null if it does not exist.
void *linkedListGet(LinkedList *linkedList, int index);
// Creates a new node containing the supplied data and returns it.
struct Node *linkedListcreateNode(LinkedList *linkedList, void *data);
// Inserts a new element at the end of the list and returns the stored data pointer.
void *linkedListinsertAtEnd(LinkedList *linkedList, void *data);
// Inserts a new element at the beginning of the list and returns the stored data pointer.
void *linkedListinsertAtFirst(LinkedList *linkedList, void *data);
// Inserts a new element at the requested position, shifting later elements forward.
void *linkedListinsertAtPosition(LinkedList *linkedList, void *data, int position);

// Inserts data into the list using the supplied comparison callback to keep it sorted. The callback should return <= 0 to insert before the current node.
void *linkedListinsertSorted(LinkedList *linkedList, void *data, int (*compare)(void *newItem, void *oldItems));

// Removes the first node in the list and returns 0 on success or -1 when the list is empty.
int linkedListdeleteFromFirst(LinkedList *linkedList);
// Removes the last node in the list and returns 0 on success or -1 when the list is empty.
int linkedListdeleteFromEnd(LinkedList *linkedList);
// Removes every node from the list. The current implementation does not return a status code.
int linkedListdeleteAll(LinkedList *linkedList);

// Removes the node at the requested position and returns 0 on success, -1 for an empty list, or -2 for an out-of-range position.
int linkedListdeleteAtPosition(LinkedList *linkedList, int position);
// Copies the list contents into a contiguous array and returns it.
void *linkedListToArray(LinkedList *linkedList);
// Prints the linked list contents for debugging purposes.
void linkedListprint(LinkedList *linkedList);
#endif