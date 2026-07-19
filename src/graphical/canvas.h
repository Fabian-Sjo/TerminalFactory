#ifndef CANVAS_H
#define CANVAS_H

#include "sprite.h"
#include "..\utils\vector2.h"
#include <stdbool.h>

typedef struct Canvas Canvas;
typedef struct NineRect
{
	Sprite sprites[3][3];
} NineRect;

// Allocates a new canvas of the requested size and returns it.
// A null or invalid size is not handled specially.
Canvas *
canvasNew(Vector2Int size);
// Enables or disables double-spaced rendering for the canvas.
// This doubles the logical width when drawing and reading.
void canvasSetDoubleSpaced(Canvas *canvas, bool doDoubleSpace);
// Returns whether the canvas is configured to render with double spacing.
// Null canvases report false.
bool canvasGetDoubleSpaced(Canvas *canvas);
// Returns the display scale used for rendering.
// Normal canvases return {1,1}; double-spaced canvases return {2,1}.
Vector2Int canvasGetDisplayScale(Canvas *canvas);

// Resizes the canvas to the provided dimensions.
// Existing contents are not preserved beyond the new allocation.
void canvasSetSize(Canvas *canvas, Vector2Int size);
// Returns the current canvas dimensions.
Vector2Int canvasGetSize(Canvas *canvas);

// Writes a sprite into the canvas at the specified position.
// Out-of-bounds positions are ignored.
void canvasSetSprite(Canvas *canvas, Vector2Int pos, Sprite sprite);
// Reads the sprite stored at the requested position.
// In double-spaced mode, odd x coordinates return a blank sprite.
Sprite canvasGetSprite(Canvas *canvas, Vector2Int pos);

// Frees the memory used by the canvas.
// The current implementation only declares this function and does not provide a body.
void canvasRemove(Canvas *canvas);
enum FILL_MODE
{
	FILL_NONE,
	FILL_ALL
};
// Fills the entire canvas with the provided sprite.
// The canvas must not be null.
canvasFill(Canvas *canvas, Sprite sprite);
// Copies a rectangular area from one canvas into another at the specified destination position.
// Double-spaced source canvases are expanded into two destination cells per source cell.
void canvasCopyToCanvas(
	Canvas *destination,
	Vector2Int destinationPos,
	Canvas *source,
	Vector2Int sourceStart,
	Vector2Int Size);

// Draws a rectangle filled or outlined according to the provided fill mode.
// The border is drawn when FILL_NONE is used and the interior is drawn when FILL_ALL is used.
void canvasDrawRectangle(Canvas *canvas, Vector2Int pos, Vector2Int size, Sprite sprite, enum FILL_MODE fillMode);
// Draws a 3x3 nine-slice rectangle using the supplied corner, edge, and center sprites.
// The center sprite is skipped unless FILL_ALL is used.
void canvasDrawNineRect(Canvas *canvas, Vector2Int pos, Vector2Int size, NineRect nineRect, enum FILL_MODE fillMode);
// Converts the canvas contents into a printable string representation.
// The returned buffer contains a newline after every row and is null-terminated.
char *canvasToString(Canvas *canvas, bool respectDoubleSpace);

struct TextFormat
{
	Vector2Int textBoxSize;
	Color foreground;
	Color background;
	bool breakOnWords;
	bool leftAlign;
	bool trimStartWhitespace;
};
// Writes a string into the canvas using the supplied text box bounds and colors.
// The return value is the bounding box of the written area and stops once the box height is exceeded or the canvas bounds are reached.
Vector2Int canvasWriteString(Canvas *canvas, char *str, Vector2Int pos, struct TextFormat *format);
#endif