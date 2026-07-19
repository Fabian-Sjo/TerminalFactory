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
	if (canvas == NULL)
		return false;
	return canvas->isDoubleSpaced;
}
Vector2Int canvasGetDisplayScale(Canvas *canvas)
{
	if (canvas == NULL)
		return (Vector2Int){1, 1};
	return (Vector2Int){1 + canvas->isDoubleSpaced, 1};
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

	if (canvas == NULL || pos.x >= canvas->size.x || pos.y >= canvas->size.y)
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
canvasFill(Canvas *canvas, Sprite sprite)
{
	for (size_t i = 0; i < canvas->size.x * canvas->size.y; i++)
	{
		canvas->sprites[i] = sprite;
	}
}
void canvasCopyToCanvas(
	Canvas *destination,
	Vector2Int destinationStart,
	Canvas *source,
	Vector2Int sourceStart,
	Vector2Int size)
{

	for (size_t x = 0; x < size.x; x++)
	{
		for (size_t y = 0; y < size.y; y++)
		{
			Sprite sprite = source->sprites[sourceStart.x + x + (sourceStart.y + y) * source->size.x];
			if (source->isDoubleSpaced)
			{
				canvasSetSprite(
					destination,
					(Vector2Int){
						(destinationStart.x) + x * 2 + 1,
						(destinationStart.y) + y,
					},
					(Sprite){' '});
				canvasSetSprite(
					destination,
					(Vector2Int){
						(destinationStart.x) + x * 2,
						(destinationStart.y) + y,
					},
					sprite);
			}
			else
				canvasSetSprite(
					destination,
					(Vector2Int){
						(destinationStart.x) + x,
						(destinationStart.y) + y,
					},
					sprite);
		}
	}
}

void canvasDrawRectangle(Canvas *canvas, Vector2Int pos, Vector2Int size, Sprite sprite, enum FILL_MODE fillMode)
{

	for (int x = 0; x < size.x; x++)
	{
		for (int y = 0; y < size.y; y++)
		{

			if (fillMode == FILL_NONE)
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
char *canvasToString(Canvas *canvas, bool respectDoubleSpace)
{
	size_t sizeX = canvas->size.x;
	if (canvas->isDoubleSpaced && respectDoubleSpace)
		sizeX *= 2;
	char *string = malloc(sizeof(char) * sizeX * canvas->size.y + canvas->size.y);
	int i = 0;
	for (size_t y = 0; y < canvas->size.y; y++)
	{
		for (size_t x = 0; x < sizeX + 1; x++)
		{
			char icon = '\n';
			if (x < sizeX)
				icon = canvasGetSprite(canvas, (Vector2Int){x, y}).icon;
			if (icon == 0)
				icon = ' ';
			switch (icon)
			{
			case ' ':
				printf("_");
				break;
			case '\n':
				printf("\\n\n");
				break;

			default:
				printf("%c", icon);
				break;
			}
			string[i++] = icon;
		}
	}
	string[i - 1] = 0;
	return string;
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
Vector2Int canvasWriteString(Canvas *canvas, char *str, Vector2Int pos, Vector2Int textBoxSize, Color foreground, Color background)
{
	int i = -1;
	Vector2Int finalBoundingBox = {0};
	while (str[++i])
	{
		Vector2Int textBoxPos = {i % textBoxSize.x, i / textBoxSize.x};
		Vector2Int thisPos = vecAddI(textBoxPos, pos);

		int offset = thisPos.x + thisPos.y * canvas->size.x;
		if (textBoxPos.y >= textBoxSize.y)
			return finalBoundingBox;
		if (offset >= canvas->size.x * canvas->size.y)
			return finalBoundingBox;
		finalBoundingBox = (Vector2Int){
			max(finalBoundingBox.x, textBoxPos.x + 1),
			max(finalBoundingBox.y, textBoxPos.y + 1)};
		canvas->sprites[offset] = (Sprite){
			.icon = str[i],
			.colorFore = foreground,
			.colorBack = background};
	}
	return finalBoundingBox;
}