#include "tiles.h"
#include <stdlib.h>

#include "../gameData.h"
// TODO can use item pooling

// example of specific tile
typedef struct ConveyorInstance
{
	short direction;
} ConveyorInstance;
void conveyorTick(int instanceID, GameData *gameData)
{
	ConveyorInstance *instance = (ConveyorInstance *)worldTileFromInstanceID(gameData->activeWorld, instanceID);
	instance->direction++;
};
Sprite conveyorSprite(int instanceID, Vector2Int offset, GameData *gameData)
{
	ConveyorInstance *instance = (ConveyorInstance *)worldTileFromInstanceID(gameData->activeWorld, instanceID);
	if (instance->direction % 2)
		return (Sprite){'7'};
	return (Sprite){'-'};
};
void conveyorInit(Tile tile, GameData *gameData) {

};
void conveyorDestroy(int instanceID, GameData *gameData) {

};

Sprite errorSprite(int instanceID, Vector2Int offset, GameData *gameData)
{
	return (Sprite){'X', COLOR_RED, COLOR_BLACK};
};

const TileDefinition TILE_DEFS[TILE_COUNT] = {
	[TILE_ERROR] = {
		.getSprite = &errorSprite,
		.tick = &conveyorTick,
		.init = &conveyorInit,
		.destroy = &conveyorDestroy,
		.sizeOfInstance = sizeof(ConveyorInstance),
		.name = {'E', 'R', 'R', 'O', 'R', '\0'},
		.icon = '0',
	},
	[TILE_BELT] = {
		.getSprite = &conveyorSprite,
		.tick = &conveyorTick,
		.init = &conveyorInit,
		.destroy = &conveyorDestroy,
		.sizeOfInstance = sizeof(ConveyorInstance),
		.name = {'B', 'E', 'L', 'T', '\0'},
		.icon = '>',
	},
};
TileDefinition *getTileDefinition(TileKind kind)
{
	if (kind < 0 || kind >= TILE_COUNT)
		return NULL;
	return &TILE_DEFS[kind];
}
void tileInit(Tile *tile, Vector2Int pos, int instanceID, GameData *gameData) {

};
void tileDestroy(Tile *tile, Vector2Int pos, GameData *gameData) {

};

Sprite tileGetSprite(Tile *tile) {

};
