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
Sprite conveyorSprite(int instanceID, Vector2Int pos, GameData *gameData)
{
	ConveyorInstance *instance = (ConveyorInstance *)worldTileFromInstanceID(gameData->activeWorld, instanceID);
	Vector2Int offset = vecSubI(pos, worldTileOriginPosFromId(gameData->activeWorld, instanceID));
	if (gameData->tick % 2 == (pos.x + pos.y) % 2)
		return (Sprite){'X'};
	return (Sprite){'-' + offset.x};
};
void conveyorInit(Tile tile, GameData *gameData) {

};
void conveyorDestroy(int instanceID, GameData *gameData) {

};

Sprite errorSprite(int instanceID, Vector2Int pos, GameData *gameData)
{
	return (Sprite){'X', COLOR_RED, COLOR_BLACK};
};
typedef struct BigTileInstance
{
	char data;
} BigTileInstance;
void bigTileTick(int instanceID, GameData *gameData)
{
	BigTileInstance *instance = (BigTileInstance *)worldTileFromInstanceID(gameData->activeWorld, instanceID);
	instance->data++;
};
Sprite bigTileSprite(int instanceID, Vector2Int pos, GameData *gameData)
{
	BigTileInstance *instance = (BigTileInstance *)worldTileFromInstanceID(gameData->activeWorld, instanceID);
	
	char sprite[3][3] = {
		{'X', '=', 'X'},
		{'|', 'O', '|'},
		{'X', '=', 'X'},
	};
	
	Vector2Int offset = vecSubI(pos, worldTileOriginPosFromId(gameData->activeWorld, instanceID));
	return (Sprite){sprite[offset.y][offset.x], COLOR_WHITE, COLOR_BLACK}; // Edges
};

const TileDefinition TILE_DEFS[TILE_COUNT] = {
	[TILE_ERROR] = {
		.getSprite = &errorSprite,
		.tick = &conveyorTick,
		.init = &conveyorInit,
		.destroy = &conveyorDestroy,
		.size = {1, 1},
		.sizeOfInstance = sizeof(ConveyorInstance),
		.name = {'E', 'R', 'R', 'O', 'R', '\0'},
		.icon = '0',
	},
	[TILE_BELT] = {
		.getSprite = &conveyorSprite,
		.tick = &conveyorTick,
		.init = &conveyorInit,
		.destroy = &conveyorDestroy,
		.size = {1, 1},
		.sizeOfInstance = sizeof(ConveyorInstance),
		.name = {'B', 'E', 'L', 'T', '\0'},
		.icon = '>',
	},
	[TILE_BIG] = {
		.getSprite = &bigTileSprite,
		.tick = &bigTileTick,
		.init = NULL,
		.destroy = NULL,
		.size = {3, 3},
		.sizeOfInstance = sizeof(BigTileInstance),
		.name = {'B', 'I', 'G', '\0'},
		.icon = 'B',
	},
};
TileDefinition *getTileDefinition(TileKind kind)
{
	if (kind < 0 || kind >= TILE_COUNT)
		return NULL;
	return &TILE_DEFS[kind];
}
Vector2Int getTileOriginOffset(TileKind kind)
{
	TileDefinition *def = getTileDefinition(kind);
	if (def == NULL)
		return (Vector2Int){0, 0};
	return vecDivI(def->size, (Vector2Int){2, 2});
}
Vector2Int getTileSize(TileKind kind)
{
	TileDefinition *def = getTileDefinition(kind);
	if (def == NULL)
		return (Vector2Int){1, 1};
	return def->size;
}
void tileInit(Tile *tile, Vector2Int pos, int instanceID, GameData *gameData) {

};
void tileDestroy(Tile *tile, Vector2Int pos, GameData *gameData) {

};
