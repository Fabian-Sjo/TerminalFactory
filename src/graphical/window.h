#include "../utils/vector2.h"
#include "../utils/ADT.h"
#include "canvas.h"

#ifndef WINDOW_H
#define WINDOW_H

typedef struct Window Window;
void windowManagerRender(Canvas *canvas);
void windowManagerUpdate(Canvas *canvas);
bool windowManagerMouse(Vector2Int globalMousePos);

Window *windowManagerAddWindow(Window window);
Window *windowManagerGetWindow(int id);
void windowManagerRemoveWindow(int id);
void windowManagerRemoveAllWindows();

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
