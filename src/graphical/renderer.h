#ifndef RENDERER_H
#define RENDERER_H

#include "sprite.h"
#include "canvas.h"

#include "..\utils\vector2.h"


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
