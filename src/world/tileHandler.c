#include "tileHandler.h"
#include "tiles.h"

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

	int instanceID = handler->count++;
	handler->instances[instanceID].kind = kind;
	handler->instances[instanceID].data = malloc(def->sizeOfInstance);
	Tile tile = {.kind = kind, .instanceID = instanceID, .pos = pos};
	def->init(tile, gameData);
	return tile;
}
void destroyFunctionTile(TileHandler *handler, int instanceID)
{
}
void tickFunctionTiles(TileHandler *handler, GameData *gameData)
{
	for (int i = 0; i < handler->count; i++)
	{
		struct TileDefinition *def = getTileDefinition(handler->instances[i].kind);
		def->tick(handler->instances[i].data, gameData);
	}
}