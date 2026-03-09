#include "graphical/sprite.h"

static Sprite sprites[SPRITE_COUNT] =
	{
		{' ', {10, 10, 10}, {10, 10, 10}}, // EMPTY
		{'-', {0, 0, 100}, {0, 0, 0}},	   // WATER
		{'~', {0, 0, 0}, {0, 0, 100}},	   // WATER
		{'B', {196, 194, 74}, {0, 0, 0}},  // SAND
		{'&', {0, 55, 0}, {0, 0, 0}},	   // GRASS
		{'#', {40, 40, 40}, {0, 0, 0}},	   // stone
		{'@', {200, 200, 200}, {0, 0, 0}}	   // SNOW
};

Sprite *getSprite(SpriteID id)
{
	return &sprites[id];
}