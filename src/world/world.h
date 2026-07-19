#ifndef WORLD_H
#define WORLD_H

#include "tiles.h"
#include "groundTile.h"
#include "../utils/vector2.h"
#include "../graphical/renderer.h"
#include "chunk.h"

typedef struct World World;

// Allocates and initializes a new world instance.
World *createWorld();
// Advances the world simulation by one tick using the provided game data.
void worldTick(GameData *gameData);

// Resolves a tile pointer from a tile instance id.
void *worldTileFromInstanceID(World *world, int instanceID);
// Returns the origin position of the tile associated with the instance id.
Vector2Int worldTileOriginPosFromId(World *world, int instanceID);
// Returns the direction stored for the tile associated with the instance id.
Direction worldTileDirFromId(World *world, int instanceID);
// Returns true when the chunk at the requested coordinates has already been generated.
int chunkIsGenerated(World *world, int x, int y);
// Generates a chunk at the given coordinates using the provided generator callback.
void generateChunk(World *world, int x, int y, Chunk generatorFunc(Vector2Int chunkCoords));

// Returns the number of currently loaded chunks.
int nrOfChunks(World *world);

// Writes the world area into the canvas at the requested offset using the provided game data.
void writeAreaToCanvas(World *world, Canvas *canvas, Vector2Int posA, Vector2Int size, Vector2Int canvasPos, GameData *gameData);

// Returns the tile at the requested world coordinates, or null if none exists.
Tile *getTile(World *world, int x, int y);
// Returns whether the tile at the given position is walkable.
bool isTileWalkable(World *world, Vector2Int pos);
// Returns the ground tile at the requested coordinates.
GroundTile *getGroundTile(World *world, int x, int y);

// Attempts to place a tile and returns 0 on success or -1 if the placement is blocked or invalid. Functional tiles are registered in the world tile handler.
int placeTile(World *world, Vector2Int position, Direction dir, TileKind tileKind);
// Removes the tile at the provided world position, including all cells covered by a multi-tile.
void removeTile(World *world, Vector2Int position);
// Replaces the ground tile at the requested coordinates. The chunk for the position must already exist.
void setGroundTile(World *world, int x, int y, GroundTile tile);

// Returns true when the tile can be placed at the given position, otherwise false.
int canPlaceTile(World *world, Vector2Int position, TileKind tileKind);
// Returns true when a multi-tile can be placed at the given position, otherwise false.
int canPlaceMultiTile(World *world, Vector2Int position, TileKind tileKind);

#endif
