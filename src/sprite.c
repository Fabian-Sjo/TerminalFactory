#include "sprite.h"

static Sprite sprites[SPRITE_COUNT] =
	{
		{' ', {10, 10, 10}, {10, 10, 10}}, // EMPTY
		{' ', {0, 0, 100}, {0,0,60}}, // WATER
		{' ', {0, 0, 100}, {0,0,90}}, // WATER
		{' ', {196, 194, 74}, {112, 111, 43}},  // SAND
		{' ', {0, 55, 0}, {0,50,0}}  // GRASS
};

Sprite *getSprite(SpriteID id)
{
	return &sprites[id];
}