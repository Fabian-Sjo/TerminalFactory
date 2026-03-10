#ifndef RENDERER_H
#define RENDERER_H


#include "sprite.h"



		
void addStrToBuffer(char *str);
void addCharToBuffer(char c);
void addSpriteToBuffer(SpriteID sprite);

void setColorFore(Color colorForeground);
void setColorBack(Color colorBackground);
void flush();
void newLine();
#endif

