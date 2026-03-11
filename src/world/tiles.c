#include "tiles.h"

typedef struct
{
	Sprite sprite;
	int state;
} SingleEntityData;

typedef struct
{
	int offsetX;
	int offsetY;
	Tile *originTile;
} MultiPartData;

typedef struct
{
	int sizeX;
	int sizeY;
	int state;
	Sprite **sprites;
} MultiOriginData;

struct Tile
{
	TileType type;
	union
	{
		SingleEntityData single;
		MultiPartData part;
		MultiOriginData origin;
	};
};

Tile testTile = {
	.type = TILE_SINGLE_ENTITY,
	.single = {.sprite = {'T', {0, 200, 0}, COLOR_BLACK}, .state = 0}};

TileType getTileType(Tile *tile)
{
}
Sprite getMulitTileSprite(Tile *originTile, int offsetX, int offsetY)
{
	Sprite **sprites = originTile->origin.sprites;
	return (sprites[offsetX][offsetY]);
}
Sprite getTileSprite(Tile *tile)
{
	if(tile != NULL){
		int a = 1;
	}
	switch (tile->type)
	{
	case TILE_SINGLE_ENTITY:
		return tile->single.sprite;
	case TILE_MULTI_PART:
		return getMulitTileSprite(
			tile->part.originTile,
			tile->part.offsetX,
			tile->part.offsetY);
	case TILE_MULTI_CORE:
		return getMulitTileSprite(tile, 0, 0);
		break;

	default:
		Sprite sprite = {'X', {200, 200, 0}, COLOR_BLACK};
		return sprite;
		break;
	}
}