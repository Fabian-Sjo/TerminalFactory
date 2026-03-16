#include "tiles.h"
#include <stdlib.h>

#include "../gameData.h"
// TODO can use item pooling

// example of specific tile
typedef struct ConveyorInstance
{
	short direction;
} ConveyorInstance;
void ConveyorTick(int instanceID, GameData *gameData) {
	ConveyorInstance *instance = (ConveyorInstance *)worldTileFromInstanceID(gameData->activeWorld, instanceID);
	instance->direction++;
};
Sprite ConveyorSprite(int instanceID, Vector2Int offset, GameData *gameData)
{
	ConveyorInstance *instance = (ConveyorInstance *)worldTileFromInstanceID(gameData->activeWorld, instanceID);
	if (instance->direction % 2)
		return (Sprite){'7'};
	return (Sprite){'-'};
};
void ConveyorInit(Tile tile, GameData *gameData) {

};
void ConveyorDestroy(int instanceID, GameData *gameData) {

};

const TileDefinition TILE_DEFS[TILE_COUNT] = {
	[TILE_BELT] = {
		.getSprite = &ConveyorSprite,
		.tick = &ConveyorTick,
		.init = &ConveyorInit,
		.destroy = &ConveyorDestroy,
		.sizeOfInstance = sizeof(ConveyorInstance)},
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
