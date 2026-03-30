#ifndef SPRITE_H
#define SPRITE_H

#include "color.h"

typedef struct Sprite
{
	int icon;
	Color colorFore;
	Color colorBack;
} Sprite;

typedef enum SpriteId
{
	SPRITE_EMPTY,
	SPRITE_WATER_DEEP,
	SPRITE_WATER,
	SPRITE_SAND,
	SPRITE_GRASS,
	SPRITE_STONE,
	SPRITE_SNOW,
	SPRITE_COUNT
} SpriteID;

Sprite *getSprite(SpriteID id);

#endif
