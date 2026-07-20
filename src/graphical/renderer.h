#ifndef RENDERER_H
#define RENDERER_H

#include "sprite.h"
#include "canvas.h"

#include "..\utils\vector2.h"

void rendererAddUTF8ToBuffer(UTF8 c);
// Draws the provided canvas through the renderer.
void rendererDrawCanvas(Canvas *canvas);
// Flushes the renderer's pending draw operations to the terminal or output target.
void rendererFlush();

#endif
