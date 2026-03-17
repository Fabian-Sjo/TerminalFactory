#ifndef WORLD_H
#define WORLD_H

#include "tiles.h"
#include "groundTile.h"
#include "../utils/vector2.h"
#include "../graphical/renderer.h"

typedef struct World World;

World *createWorld();
void worldTick(GameData *gameData);

void *worldTileFromInstanceID(World *world, int instanceID);
Vector2Int worldTileOriginPosFromId(World *world, int instanceID);

int chunkIsGenerated(World *world, int x, int y);
void generateChunk(World *world, int x, int y);

int nrOfChunks(World *world);

void writeAreaToCanvas(World *world, Canvas *canvas, Vector2Int posA, Vector2Int size, Vector2Int canvasPos, GameData *gameData);

Tile *getTile(World *world, int x, int y);

GroundTile *getGroundTile(World *world, int x, int y);

// returns 0 if successful, negative if failed
int placeTile(World *world, Vector2Int position, TileKind tileKind);
void removeTile(World *world, Vector2Int position);
void setGroundTile(World *world, int x, int y, GroundTile tile);

int canPlaceTile(World *world, Vector2Int position, TileKind tileKind);
int canPlaceMultiTile(World *world, Vector2Int position, TileKind tileKind);

#endif
