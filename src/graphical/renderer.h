#ifndef RENDERER_H
#define RENDERER_H

#include "sprite.h"
#include "..\utils\vector2.h"

typedef struct Canvas
{
	Vector2Int size;

	// 2d array packed in to 1D
	// sprites[x + y * width]
	Sprite *sprites;
} Canvas;
Canvas *getCanvas(Vector2Int size);

void addCanvasToBuffer(Canvas *canvas);

void addStrToBuffer(char *str);
void addCharToBuffer(char c);
void addSpriteIdToBuffer(SpriteID sprite);
void addSpriteToBuffer(Sprite sprite);

void setColorFore(Color colorForeground);
void setColorBack(Color colorBackground);
void flush();
void newLine();
#endif
