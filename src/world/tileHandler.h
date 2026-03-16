#ifndef TILEHANDLER_H
#define TILEHANDLER_H
#include "tiles.h"

#define MAX_TILES 1024

struct TileInstance
{
	TileKind kind;
	void *data;
};
typedef struct TileHandler
{
	struct TileInstance instances[MAX_TILES];
	int count;
} TileHandler;

// returns the tile
Tile createFunctionTile(TileHandler *handler, TileKind kind, Vector2Int pos);
void destroyFunctionTile(TileHandler *handler, int instanceID);
void tickFunctionTiles(TileHandler *handler, GameData *gameData);
#endif