#ifndef TILEHANDLER_H
#define TILEHANDLER_H
#include "tiles.h"

#define MAX_FUNCTION_TILES 1024

struct TileInstance
{
	TileKind kind;
	Vector2Int pos;
	Direction direction;
	void *data;
};
typedef struct TileHandler
{
	struct TileInstance instances[MAX_FUNCTION_TILES];
	int count;
} TileHandler;

// Creates a functional tile instance and returns the tile descriptor to add to the world. If the tile kind has no sprite callback, an error tile is returned instead.
Tile createFunctionTile(TileHandler *handler, TileKind kind, Vector2Int pos, Direction dir, GameData *gameData);
// Creates a multi-tile instance that shares the provided origin id. The returned tile uses the origin tile's functional state.
Tile createMultiTile(TileHandler *handler, TileKind kind, Vector2Int pos, Direction dir, int originID);

// Removes the functional tile instance identified by instanceID. Negative ids are ignored.
void destroyFunctionTile(TileHandler *handler, int instanceID);
// Advances all functional tiles by one tick using the provided game data. Tiles without a tick callback are skipped.
void tickFunctionTiles(TileHandler *handler, GameData *gameData);
#endif