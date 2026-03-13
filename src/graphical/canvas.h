#ifndef CANVAS_H
#define CANVAS_H

#include "sprite.h"
#include "..\utils\vector2.h"

typedef struct Canvas Canvas;
typedef struct NineRect
{
	Sprite sprites[3][3];
} NineRect;
Canvas *canvasNew(Vector2Int size);

void canvasSetSize(Canvas *canvas, Vector2Int size);
Vector2Int canvasGetSize(Canvas *canvas);

void canvasSetSprite(Canvas *canvas, Vector2Int pos, Sprite sprite);
Sprite canvasGetSprite(Canvas *canvas, Vector2Int pos);

void canvasRemove(Canvas *canvas);
enum FILL_MODE
{
	FILL_NONE,
	FILL_ALL
};
void cavasDrawRectangle(Canvas *canvas, Vector2Int pos, Vector2Int size, Sprite sprite, enum FILL_MODE fillMode);
void canvasDrawNineRect(Canvas *canvas, Vector2Int pos, Vector2Int size, NineRect nineRect, enum FILL_MODE fillMode);
#endif