#ifndef WORLD_H
#define WORLD_H

#include "tiles.h"
#include "groundTile.h"
#include "../utils/vector2.h"
#include "../graphical/renderer.h"

typedef struct World World;

World *createWorld();
int chunkIsGenerated(World *world, int x, int y);
void generateChunk(World *world, int x, int y);

int nrOfChunks(World *world);

void writeAreaToCanvas(World *world, Canvas *canvas, Vector2Int posA, Vector2Int size, Vector2Int canvasPos);

Tile *getTile(World *world, int x, int y);

GroundTile *getGroundTile(World *world, int x, int y);

void setTile(World *world, Vector2Int position, Tile *tile);

void setGroundTile(World *world, int x, int y, GroundTile tile);

#endif
