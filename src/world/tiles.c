#include "tiles.h"
#include <stdlib.h>

#include "../gameData.h"
// TODO can use item pooling

/// General tile definition
struct TileDefinition
{
	Sprite (*getSprite)(int instanceID);
	void (*tick)(int instanceID, GameData *gameData);
	void (*init)(int instanceID, Vector2Int pos, GameData *gameData);
	void (*destroy)(int instanceID, GameData *gameData);
};

struct Tile
{
	struct TileDefinition *defention;
	int instanceID;
};
// example of specific tile

void ConveyorTick(int instanceID, GameData *gameData)
{
	//instance->direction++;
};
Sprite ConveyorSprite(int instanceID)
{
	//if (instance->direction % 2)
		return (Sprite){'7'};
	return (Sprite){'-'};
};
void ConveyorInit(Tile *tile, Vector2Int pos, GameData *gameData)
{
	
};
void ConveyorDestroy(Tile *tile, GameData *gameData)
{
	
};
struct TileDefinition ConveyorDefinition = {
	.getSprite = &ConveyorSprite,
	.tick = &ConveyorTick,
	.init = &ConveyorInit,
	.destroy = &ConveyorDestroy};

Tile TILE_CONVEYOR = {
	.defention = &ConveyorDefinition,
	.instanceID = -1};

void tileInit(Tile *tile, Vector2Int pos, GameData *gameData)
{
	tile->defention->init(tile, pos, gameData);
};
void tileDestroy(Tile *tile, Vector2Int pos, GameData *gameData)
{
	tile->defention->destroy(tile, gameData);
};

Sprite tileGetSprite(Tile *tile)
{
	return tile->defention->getSprite(tile->instanceID);
};
