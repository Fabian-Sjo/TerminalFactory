#ifndef RENDERER_H
#define RENDERER_H

#include "sprite.h"
#include "canvas.h"

#include "..\utils\vector2.h"

typedef struct
{
	Vector2Int pos;
} SubWindowSettings;


// Draws the provided canvas through the renderer.
void rendererDrawCanvas(Canvas *canvas);
// Flushes the renderer's pending draw operations to the terminal or output target.
void rendererFlush();

#endif
