#ifndef GROUNDTILE_H
#define GROUNDTILE_H
#include "../graphical/sprite.h"

typedef struct GroundTile
{
	Sprite sprite;
} GroundTile;

extern GroundTile GROUND_TILE_WATER;
extern GroundTile GROUND_TILE_GRASS_1;
extern GroundTile GROUND_TILE_GRASS_2;
extern GroundTile GROUND_TILE_GRASS_3;

Sprite getGroundTileSprite(GroundTile *groundTile);

#endif