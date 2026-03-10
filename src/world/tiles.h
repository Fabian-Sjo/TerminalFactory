#ifndef TILES_H
#define TILES_H

#include "../graphical/sprite.h"

typedef enum
{
	TILE_NONE,
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
typedef struct
{
	Sprite sprite;
	int state;
} SingleEntityData;

typedef struct
{
	int originX;
	int originY;
} MultiPartData;

typedef struct
{
	Sprite *sprites;
	int spriteCount;
	int state;
} MultiOriginData;

typedef struct Tile
{
	TileType type;
	union
	{
		SingleEntityData single;
		MultiPartData part;
		MultiOriginData origin;
	};

} Tile;

extern Tile testTile;

void getTileType();
void getTileSprite();
void getTileState();

#endif
