#include "window.h"
#include "stdlib.h"

LinkedList *activeWindows = NULL;

Vector2Int localizeMousePos(Vector2Int mousePos, Window window)
{
	if (window.parent != NULL)
		mousePos = localizeMousePos(mousePos, *window.parent);

	Vector2Int scale = window.scale;
	return (Vector2Int){
		(mousePos.x - window.position.x) / scale.x,
		(mousePos.y - window.position.y) / scale.y};
}

bool isMouseInBounds(Vector2Int mousePos, Window window)
{
	Vector2Int localMousePos = localizeMousePos(mousePos, window);
	return (localMousePos.x >= 0 &&
			localMousePos.y >= 0 &&
			localMousePos.x < window.size.x &&
			localMousePos.y < window.size.y);
}

void windowManagerRender(Canvas *canvas)
{
	for (size_t i = 0; i < activeWindows->nrOfElements; i++)
	{
		Window *thisWindow = (Window *)linkedListGet(activeWindows, i);
		printf("%d", (thisWindow)->z);
		thisWindow->render(thisWindow, canvas);
	}
}
void windowManagerUpdate(Canvas *canvas)
{

	for (size_t i = 0; i < activeWindows->nrOfElements; i++)
	{
		Window *thisWindow = (Window *)linkedListGet(activeWindows, i);
		if (thisWindow->update != NULL)
			thisWindow->update(thisWindow);
	}
}
bool windowManagerMouse(Vector2Int globalMousePos)
{
	for (size_t i = 0; i < activeWindows->nrOfElements; i++)
	{
		Window *thisWindow = (Window *)linkedListGet(activeWindows, activeWindows->nrOfElements - 1 - i);
		if (!isMouseInBounds(globalMousePos, *thisWindow))
			continue;
		if (thisWindow->mouse == NULL)
			continue;
		Vector2Int localMousePos = localizeMousePos(globalMousePos, *thisWindow);
		bool wasConsumed = thisWindow->mouse(thisWindow, localMousePos, globalMousePos);

		if (wasConsumed)
		{
			return thisWindow->z;
		}
	}

	return false;
}
static int compareWindows(void *newItem, void *oldItem)
{
	Window *a = (Window *)newItem;
	Window *b = (Window *)oldItem;
	return a->z - b->z;
}
Window *windowManagerAddWindow(Window window)
{
	if (activeWindows == NULL)
		activeWindows = linkedListCreate(sizeof(Window));

	return (Window *)linkedListinsertSorted(activeWindows, &window, compareWindows);
}
Window *windowManagerGetWindow(int id)
{
	for (size_t i = 0; i < activeWindows->nrOfElements; i++)
	{
		Window *thisWindow = (Window *)linkedListGet(activeWindows, i);
		if (thisWindow->id == id)
			return thisWindow;
	}
	return NULL;
}
void windowManagerRemoveWindow(int id)
{
}
void windowManagerRemoveAllWindows()
{
}