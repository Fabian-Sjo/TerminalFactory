#ifndef TILES_H
#define TILES_H

#include "../graphical/sprite.h"

typedef enum
{
	TILE_MULTI_PART,
	TILE_MULTI_CORE,
	TILE_SINGLE_ENTITY
} TileType;

/*
has type
	EMPTY(type)
	has data
		PART OF MULTI (parent, type(gets real type from parent))
		has state
			SINGLE (1 sprite, state, type)
			MULTI CORE (n sprites, state, type)
			*/
typedef struct Tile Tile;

extern Tile testTile;

TileType getTileType(Tile *tile);
Sprite getTileSprite(Tile *tile);
void getTileState();

#endif
