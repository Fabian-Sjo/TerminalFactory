#include "tileHandler.h"
#include "tiles.h"

Tile createFunctionTile(TileHandler *handler, TileKind kind, Vector2Int pos)
{
	// handler
	struct TileDefinition *def = getTileDefinition(kind);
	// if (def == TILE_NONE) return -1;

	int instanceID = handler->count++;
	handler->instances[instanceID].kind = kind;
	handler->instances[instanceID].data = malloc(getTileInstaceSize(def));
	def->init(handler->instances[instanceID].data, pos, instanceID, NULL);
	return (Tile){.kind = kind, .instanceID = instanceID, .pos = pos};
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