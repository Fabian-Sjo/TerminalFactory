#ifndef TILES_H
#define TILES_H

#include "../graphical/sprite.h"
#include "../utils/vector2.h"

typedef struct World World;
typedef struct GameData GameData;
typedef struct Tile
{
	struct TileDefinition *defention;
	int instanceID;
	Vector2Int pos;
}Tile;


Tile TILE_CONVEYOR;
struct ConveyorInstance
{
	short direction;
};


void tileInit(Tile *tile, Vector2Int pos, GameData *GameData);
void tileDestroy(Tile *tile, Vector2Int pos, GameData *GameData);

Sprite tileGetSprite(Tile *tile);

#endif
