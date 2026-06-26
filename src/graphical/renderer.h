#ifndef RENDERER_H
#define RENDERER_H

#include "sprite.h"
#include "canvas.h"

#include "..\utils\vector2.h"

typedef struct
{
	Vector2Int pos;
} SubWindowSettings;


int rendererAddCanvas(Canvas *canvas);
void rendererDrawCanvas(Canvas *canvas);
void rendererFlush();

#endif
