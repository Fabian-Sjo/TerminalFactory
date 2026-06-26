#include "canvas.h"
#include "stdlib.h"
#include <stdbool.h>
typedef struct Canvas
{
	Vector2Int size;
	bool isDoubleSpaced;
	// 2d array packed in to 1D
	// sprites[x + y * width]
	Sprite *sprites;
} Canvas;

Canvas *canvasNew(Vector2Int size)
{
	Canvas *newCanvas = malloc(sizeof(Canvas));
	newCanvas->size = size;
	newCanvas->sprites = calloc(size.x * size.y, sizeof(Sprite));
	newCanvas->isDoubleSpaced = false;
	return newCanvas;
}
void canvasSetDoubleSpaced(Canvas *canvas, bool doDoubleSpace)
{
	canvas->isDoubleSpaced = doDoubleSpace;
}
bool canvasGetDoubleSpaced(Canvas *canvas)
{
	return canvas->isDoubleSpaced;
}
void canvasSetSize(Canvas *canvas, Vector2Int size)
{
	if (
		size.x == canvas->size.x &&
		size.y == canvas->size.y)
	{
		return;
	}
	canvas->sprites = realloc(canvas->sprites, size.x * size.y * sizeof(Sprite));
	//		Sprite *newSprites = calloc(size.x * size.y, sizeof(Sprite));
	//	;
	//	free(canvas->sprites);
	//	canvas->sprites = newSprites;
	canvas->size = size;
}
Vector2Int canvasGetSize(Canvas *canvas)
{
	return canvas->size;
}

void canvasSetSprite(Canvas *canvas, Vector2Int pos, Sprite sprite)
{

	if (pos.x >= canvas->size.x || pos.y >= canvas->size.y)
		return;
	Sprite spriteToSave = canvas->sprites[pos.x + pos.y * canvas->size.x];
	if (!colorEquals(sprite.colorFore, COLOR_TRANSPARENT))
		spriteToSave.colorFore = sprite.colorFore;
	if (!colorEquals(sprite.colorBack, COLOR_TRANSPARENT))
		spriteToSave.colorBack = sprite.colorBack;
	spriteToSave.icon = sprite.icon;

	canvas->sprites[pos.x + pos.y * canvas->size.x] = spriteToSave;
}
Sprite canvasGetSprite(Canvas *canvas, Vector2Int pos)
{
	if (canvas->isDoubleSpaced && (pos.x & 1))
		return (Sprite){.icon = ' '};
	return canvas->sprites[pos.x / (1 + canvas->isDoubleSpaced) + pos.y * canvas->size.x];
}

void canvasRemove(Canvas *canvas);

void cavasDrawRectangle(Canvas *canvas, Vector2Int pos, Vector2Int size, Sprite sprite, enum FILL_MODE fillMode)
{

	for (int x = 0; x < size.x; x++)
	{
		for (int y = 0; y < size.y; y++)
		{

			if (fillMode == FILL_ALL)
			{
				if (x == 0 || y == 0 || x == size.x - 1 || y == size.y - 1)
					canvasSetSprite(canvas, (Vector2Int){x + pos.x, y + pos.y}, sprite);
			}
			else
			{
				canvasSetSprite(canvas, (Vector2Int){x + pos.x, y + pos.y}, sprite);
			}
		}
	}
}
void canvasDrawNineRect(Canvas *canvas, Vector2Int pos, Vector2Int size, NineRect nineRect, enum FILL_MODE fillMode)
{

	for (int x = 0; x < size.x; x++)
	{
		for (int y = 0; y < size.y; y++)
		{
			int spriteX = (!!x) + (x == size.x - 1);
			int spriteY = (!!y) + (y == size.y - 1);
			Sprite sprite = nineRect.sprites[spriteX][spriteY];
			if (spriteX != 1 || spriteY != 1 || fillMode == FILL_ALL)
				canvasSetSprite(canvas, (Vector2Int){x + pos.x, y + pos.y}, sprite);
		}
	}
}