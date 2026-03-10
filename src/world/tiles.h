#ifndef TILES_H
#define TILES_H

#include "../graphical/sprite.h"

typedef enum
{
	TILE_NONE,
	TILE_MULTI_PART,
	TILE_MULTI_CORE,
	TILE_ENTITY
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
typedef struct Data
{
	struct // PART OF ENTITY
	{
		int originX, originY;
	};
	struct // ENTITY
	{
		Sprite sprite;
		int state;
	};
} Data;

typedef struct Tile
{

	TileType type;
	Data *data;

} Tile;

Tile testTile = {
	TILE_ENTITY,
	{{'T', COLOR_BLACK, COLOR_BLACK}, 0}};

void getTileType();
void getTileSprite();
void getTileState();

#endif
