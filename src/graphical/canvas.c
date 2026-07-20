#include "canvas.h"
#include <stdlib.h>
#include <stdbool.h>
typedef struct Canvas
{
	Vector2Int size;
	bool isDoubleSpaced;
	bool isProxy;

	// 2d array packed in to 1D
	// sprites[x + y * width]
	Sprite *sprites;
	Canvas *sourceCanvas;
	Vector2Int sourceOffset;

} Canvas;

Canvas *canvasNew(Vector2Int size)
{
	Canvas *newCanvas = malloc(sizeof(Canvas));
	newCanvas->size = size;
	newCanvas->sprites = calloc(size.x * size.y, sizeof(Sprite));
	newCanvas->isDoubleSpaced = false;
	newCanvas->isProxy = false;
	return newCanvas;
}
Canvas *canvasProxy(Canvas *source, Rect2Int rect)
{
	Canvas *proxy = malloc(sizeof(Canvas));
	proxy->size = rectSizeI(rect);
	proxy->sourceOffset = rect.pos;
	proxy->isProxy = true;
	proxy->sourceCanvas = source;
	return proxy;
}
void canvasProxySetRect(Canvas *proxy, Rect2Int rect)
{
	if (!proxy->isProxy)
		return;
	proxy->size = rectSizeI(rect);
	proxy->sourceOffset = rect.pos;
}
void canvasFree(Canvas *canvas)
{
	if (!canvas->isProxy)
		free(canvas->sprites);
	free(canvas);
}
Canvas *canvasGetSource(Canvas *canvas)
{
	return canvas->isProxy ? canvasGetSource(canvas->sourceCanvas) : canvas;
}
Vector2Int canvasGetSourcePos(Canvas *canvas, Vector2Int pos)
{
	return canvas->isProxy ? vecAddI(canvas->sourceOffset, canvasGetSourcePos(canvas->sourceCanvas, pos)) : pos;
}
bool canvasPointInBounds(Canvas *canvas, Vector2Int pos)
{
	if (canvas == NULL || pos.x >= canvas->size.x || pos.y >= canvas->size.y || pos.x < 0 || pos.y < 0)
		return false;
	pos = canvasGetSourcePos(canvas, pos);
	canvas = canvasGetSource(canvas);
	if (canvas == NULL || pos.x >= canvas->size.x || pos.y >= canvas->size.y || pos.x < 0 || pos.y < 0)
		return false;
	return true;
}
void canvasSetDoubleSpaced(Canvas *canvas, bool doDoubleSpace)
{
	canvas->isDoubleSpaced = doDoubleSpace;
}
bool canvasGetDoubleSpaced(Canvas *canvas)
{
	canvas = canvasGetSource(canvas);
	if (canvas == NULL)
		return false;
	return canvas->isDoubleSpaced;
}
Vector2Int canvasGetDisplayScale(Canvas *canvas)
{
	canvas = canvasGetSource(canvas);
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
	if (!canvas->isProxy)
		canvas->sprites = realloc(canvas->sprites, size.x * size.y * sizeof(Sprite));
	canvas->size = size;
	// TODO check if proxy is inbounds
}
Vector2Int canvasGetSize(Canvas *canvas)
{
	return canvas->size;
}

void canvasSetSprite(Canvas *canvas, Vector2Int pos, Sprite sprite)
{
	bool proxyDoubleSpace = (canvas->isDoubleSpaced && canvas->isProxy);
	if (proxyDoubleSpace)
		pos.x *= 2;
	if (!canvasPointInBounds(canvas, pos))
		return;
	pos = canvasGetSourcePos(canvas, pos);
	canvas = canvasGetSource(canvas);

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
	if (!canvasPointInBounds(canvas, pos))
		return ERROR_SPRITE;

	pos = canvasGetSourcePos(canvas, pos);
	canvas = canvasGetSource(canvas);

	if ((canvas->isDoubleSpaced) && (pos.x & 1))
		return (Sprite){.icon = ' '};

	return canvas->sprites[pos.x / (1 + canvas->isDoubleSpaced) + pos.y * canvas->size.x];
}

canvasFill(Canvas *canvas, Sprite sprite)
{
	for (size_t x = 0; x < canvas->size.x; x++)
		for (size_t y = 0; y < canvas->size.y; y++)
			canvasSetSprite(canvas, (Vector2Int){x, y}, sprite);
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
			// Sprite sprite = source->sprites[sourceStart.x + x + (sourceStart.y + y) * source->size.x];
			Sprite sprite = canvasGetSprite(source, vecAddI(sourceStart, (Vector2Int){x, y}));
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
	char *string = malloc(sizeof(UTF8) * sizeX * canvas->size.y + canvas->size.y);
	int i = 0;
	for (size_t y = 0; y < canvas->size.y; y++)
	{
		for (size_t x = 0; x < sizeX + 1; x++)
		{
			UTF8 icon;
			if (x < sizeX)
				icon = canvasGetSprite(canvas, (Vector2Int){x, y}).icon;
			if (icon.data[0] == 0)
				icon.data[0] = ' ';
			switch (icon.data[0])
			{
			case ' ':
				printf("_");
				break;
			case '\n':
				printf("\\n\n");
				break;

			default:
				printf("%s", icon);
				break;
			}
			string[i++] = icon.data;
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

bool isWhiteSpace(char c)
{
	return (c == ' ' || c == '\n');
};
Vector2Int canvasWriteString(Canvas *canvas, char *str, Vector2Int pos, struct TextFormat *format)
{
	struct TextFormat fallback = {
		.textBoxSize = (Vector2Int){0, 0},
		.foreground = (Color){0},
		.background = (Color){0},
		.trimStartWhitespace = true,
		.breakOnWords = true,
		.alignment = TEXT_ALIGN_LEFT};
	if (format == NULL)
		format = &fallback;
	format->textBoxSize.x = format->textBoxSize.x > 0 ? format->textBoxSize.x : 10000000;
	format->textBoxSize.y = format->textBoxSize.y > 0 ? format->textBoxSize.y : 10000000;

	int i = -1;
	while (str[++i] != 0)
		;
	int stringLength = i;
	i = 0;

	Vector2Int finalBoundingBox = {0};
	int lineNr = 0;
	while (i < stringLength)
	{
		if (format->trimStartWhitespace)
		{
			while (i < stringLength && str[i] == ' ')
				i++;
		}
		int lineStart = i;
		int offset = 0;
		int maxLineLength = format->textBoxSize.x;

		int lastWordEnd = -1;
		int validLineEnd = -1;
		if (lineStart + maxLineLength > stringLength)
			validLineEnd = stringLength;
		else
			while (validLineEnd == -1)
			{
				if (offset >= maxLineLength)
				{
					validLineEnd = lineStart + maxLineLength;
				}
				if (lineStart + offset >= stringLength)
				{
					validLineEnd = lineStart + offset;
				}
				if (str[lineStart + offset] == '\n')
				{
					validLineEnd = lineStart + offset + 1;
					break;
				}

				if (format->breakOnWords &&
					offset > 0 &&
					!isWhiteSpace(str[lineStart + offset - 1]) &&
					isWhiteSpace(str[lineStart + offset]))
				{
					lastWordEnd = lineStart + offset;
				}
				offset++;
			}
		if (lastWordEnd != -1)
			validLineEnd = min(validLineEnd, lastWordEnd);
		size_t lineLength = validLineEnd - lineStart;

		for (size_t charIndex = 0; charIndex < lineLength; charIndex++)
		{
			Vector2Int spritePos = {pos.x + charIndex, pos.y + lineNr};

			if (format->alignment == TEXT_ALIGN_RIGHT)
				spritePos.x += maxLineLength - lineLength;
			if (format->alignment == TEXT_ALIGN_MID)
				spritePos.x += (maxLineLength - lineLength) / 2;
			if (spritePos.x >= canvas->size.x)
				break;
			if (spritePos.x < 0 && spritePos.y < 0)
				continue;
			if (lineNr >= format->textBoxSize.y)
				return finalBoundingBox;
			if (offset >= canvas->size.x * canvas->size.y)
				return finalBoundingBox;
			finalBoundingBox = (Vector2Int){
				max(finalBoundingBox.x, charIndex + 1),
				max(finalBoundingBox.y, lineNr + 1)};
			if (str[lineStart + charIndex] != '\n')
				canvasSetSprite(canvas,
								spritePos,
								(Sprite){.icon =
											 str[lineStart + charIndex],
										 .colorFore = format->foreground,
										 .colorBack = format->background});
		}
		i = validLineEnd;
		lineNr++;
	}
	return finalBoundingBox;
}