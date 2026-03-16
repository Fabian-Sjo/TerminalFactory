#include "tileHandler.h"
#include "tiles.h"
#include <stdlib.h>

Tile createFunctionTile(TileHandler *handler, TileKind kind, Vector2Int pos, GameData *gameData)
{
	// handler
	struct TileDefinition *def = getTileDefinition(kind);
	Tile noTile = {
		.kind = TILE_NONE,
		.instanceID = -1,
		.pos = pos};
	if (kind == TILE_NONE)
		return noTile;

	if (def->init == NULL || def->tick == NULL || def->getSprite == NULL)
	{
		// TODO log error
		Tile errorTile = {
			.kind = TILE_ERROR,
			.instanceID = -1,
			.pos = pos};

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
	handler->instances[instanceID].data = malloc(def->sizeOfInstance);
	Tile tile = {.kind = kind, .instanceID = instanceID, .pos = pos};
	def->init(tile, gameData);
	return tile;
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
		if(handler->instances[i].kind == TILE_NONE)
			continue;
		struct TileDefinition *def = getTileDefinition(handler->instances[i].kind);
		def->tick(i, gameData);
	}
}