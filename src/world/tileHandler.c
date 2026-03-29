#include "tileHandler.h"
#include "tiles.h"
#include <stdlib.h>

Tile createFunctionTile(TileHandler *handler, TileKind kind, Vector2Int pos, Direction dir, GameData *gameData)
{
	// handler
	struct TileDefinition *def = getTileDefinition(kind);
	Tile noTile = {
		.kind = TILE_NONE,
		.isFunctional = false,
		.pos = pos,
	};
	if (kind == TILE_NONE)
		return noTile;

	if (def->getSprite == NULL)
	{
		// TODO log error
		Tile errorTile = {
			.kind = TILE_ERROR,
			.isFunctional = false,
			.pos = pos,
		};

		return errorTile;
	}
	handler->count++;
	int nextOpenSlot = 0;
	while (handler->instances[nextOpenSlot].kind != TILE_NONE)
	{
		nextOpenSlot++;
	}

	int instanceID = nextOpenSlot;
	handler->instances[instanceID].kind = kind;
	handler->instances[instanceID].pos = pos;
	handler->instances[instanceID].direction = dir;
	handler->instances[instanceID].data = malloc(def->sizeOfInstance);
	Tile tile = {.kind = kind, .isFunctional = def->isFunctional, .entity.instanceID = instanceID, .pos = pos};
	if (def->init != NULL)
		def->init(tile, gameData);
	return tile;
}
Tile createMultiTile(TileHandler *handler, TileKind kind, Vector2Int pos, Direction dir, int originID)
{
	struct TileInstance originTile = handler->instances[originID];
	Tile multiTile = {
		.isFunctional = originTile.kind != TILE_NONE ? getTileDefinition(originTile.kind)->isFunctional : false,
		.kind = kind,
		.entity.instanceID = originID,
		.pos = pos,
	};
	return multiTile;
}
void destroyFunctionTile(TileHandler *handler, int instanceID)
{
	if (instanceID < 0)
		return;

	if (handler->instances[instanceID].data)
	{
		free(handler->instances[instanceID].data);
		handler->instances[instanceID].data = NULL;
		handler->instances[instanceID].kind = TILE_NONE;
	}
}
void tickFunctionTiles(TileHandler *handler, GameData *gameData)
{
	for (int i = 0; i < handler->count; i++)
	{
		if (handler->instances[i].kind == TILE_NONE)
			continue;
		struct TileDefinition *def = getTileDefinition(handler->instances[i].kind);
		if (def->tick != NULL)
			def->tick(i, gameData);
	}
}