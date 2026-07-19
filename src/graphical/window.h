#include "../utils/vector2.h"
#include "../utils/ADT.h"
#include "canvas.h"

#ifndef WINDOW_H
#define WINDOW_H

typedef struct Window Window;
// Renders all registered windows into the provided canvas.
void windowManagerRender(Canvas *canvas);
// Updates each window's state for the current frame.
void windowManagerUpdate(Canvas *canvas);
// Handles a mouse event at the given global position.
// The return value is the window z-order when the event is consumed and false otherwise.
bool windowManagerMouse(Vector2Int globalMousePos);

// Registers a window object and returns a pointer to the stored window instance.
Window *windowManagerAddWindow(Window window);
// Returns the window with the matching id, or null if no such window exists.
Window *windowManagerGetWindow(int id);
// Removes the window identified by id from the manager.
void windowManagerRemoveWindow(int id);
// Removes every managed window.
void windowManagerRemoveAllWindows();

// Converts a global mouse position into the local coordinate space of the provided window.
Vector2Int windowManagerLocalizeMousePos(Vector2Int mousePos, Window window);

struct Window
{
	int id;
	Vector2Int position;
	// the thing the window is contained in
	Window *parent;
	// used to set positions
	Window *anchor;
	void (*render)(Window *window, Canvas *canvas);
	// returns true if the input is consumed
	bool (*mouse)(Window *window, Vector2Int localMousePos, Vector2Int globalMousePos);
	void (*update)(Window *windows);
	Vector2Int scale;
	Vector2Int size;
	int z;
};

#endif
