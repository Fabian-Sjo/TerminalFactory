#include "tiles.h"
#include <stdlib.h>

#include "../gameData.h"
// TODO can use item pooling

// example of specific tile
void ConveyorTick(int instanceID, GameData *gameData) {
	// instance->direction++;
};
Sprite ConveyorSprite(int instanceID, Vector2Int offset, GameData *gameData)
{
	if (gameData->tick % 2)
		return (Sprite){'7'};
	return (Sprite){'-'};
};
void ConveyorInit(Tile tile, GameData *gameData) {

};
void ConveyorDestroy(int instanceID, GameData *gameData) {

};
typedef struct ConveyorInstance
{
	short direction;
} ConveyorInstance;

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
