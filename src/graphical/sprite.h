#ifndef SPRITE_H
#define SPRITE_H

#include "color.h"
#include "UTF8.h"
#define ERROR_SPRITE (Sprite){.icon = -1}

typedef struct Sprite
{
	UTF8 icon;
	Color colorFore;
	Color colorBack;
} Sprite;



#endif
