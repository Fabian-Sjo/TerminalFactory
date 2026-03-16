#ifndef TILEHANDLER_H
#define TILEHANDLER_H
#include "tiles.h"

#define MAX_FUNCTION_TILES 1024

struct TileInstance
{
	TileKind kind;
	void *data;
};
typedef struct TileHandler
{
	struct TileInstance instances[MAX_FUNCTION_TILES];
	int count;
} TileHandler;

// returns the tile it created, so it can be added to the world
Tile createFunctionTile(TileHandler *handler, TileKind kind, Vector2Int pos, GameData *gameData);
void destroyFunctionTile(TileHandler *handler, int instanceID);
void tickFunctionTiles(TileHandler *handler, GameData *gameData);
#endif