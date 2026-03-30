#include "tiles.h"
#include <stdlib.h>

#include "../gameData.h"
// TODO can use item pooling

// example of specific tile
typedef struct ConveyorInstance
{
	short data;
} ConveyorInstance;
void conveyorTick(int instanceID, GameData *gameData)
{
	ConveyorInstance *instance = (ConveyorInstance *)worldTileFromInstanceID(gameData->activeWorld, instanceID);
};
Sprite conveyorSprite(int instanceID, Vector2Int pos, GameData *gameData)
{
	World *world = gameData->activeWorld;
	ConveyorInstance *instance = (ConveyorInstance *)worldTileFromInstanceID(world, instanceID);
	Vector2Int offset = vecSubI(pos, worldTileOriginPosFromId(gameData->activeWorld, instanceID));
	Direction dir = instanceID;
	if (instanceID >= 0)
		dir = worldTileDirFromId(gameData->activeWorld, instanceID);
	char sprite;
	if (dir == DIR_NORTH)
		sprite = '^';
	else if (dir == DIR_EAST)
		sprite = '>';
	else if (dir == DIR_SOUTH)
		sprite = 'v';
	else if (dir == DIR_WEST)
		sprite = '<';
	else
		sprite = '?';
	return (Sprite){sprite};
};
void conveyorInit(Tile tile, GameData *gameData) {

};
void conveyorDestroy(int instanceID, GameData *gameData) {

};

Sprite errorSprite(int instanceID, Vector2Int pos, GameData *gameData)
{
	return (Sprite){'X', COLOR_RED, COLOR_BLACK};
};
Sprite bigTileSprite(int instanceID, Vector2Int pos, GameData *gameData)
{
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
		.isFunctional = true,
		.getSprite = &errorSprite,
		.tick = &conveyorTick,
		.init = &conveyorInit,
		.destroy = &conveyorDestroy,
		.size = {1, 1},
		.sizeOfInstance = 0,
		.name = {'E', 'R', 'R', 'O', 'R', '\0'},
		.icon = '0',
	},
	[TILE_ROCK] = {
		.isFunctional = false,
		.name = {'R', 'O', 'C', 'K', '\0'},
		.icon = 'R',
	},
	[TILE_BIG] = {
		.isFunctional = true,
		.getSprite = &bigTileSprite,
		.tick = NULL,
		.init = NULL,
		.destroy = NULL,
		.size = {3, 3},
		.sizeOfInstance = sizeof(ConveyorInstance),
		.name = {'B', 'I', 'G', '\0'},
		.icon = 'B',
	},
	[TILE_BELT] = {
		.isFunctional = true,
		.getSprite = &conveyorSprite,
		.tick = &conveyorTick,
		.init = &conveyorInit,
		.destroy = &conveyorDestroy,
		.size = {1, 1},
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
Sprite getTileSprite(Tile tile, Vector2Int pos, GameData gameData)
{
	if (tile.kind == TILE_NONE)
		return (Sprite){0};
	if (!tile.isFunctional)
		return tile.sprite;

	TileDefinition *def = getTileDefinition(tile.kind);
	if (def == NULL || def->getSprite == NULL)
		return (Sprite){'?', COLOR_RED, COLOR_BLACK};
	return def->getSprite(tile.entity.instanceID, pos, &gameData);
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
