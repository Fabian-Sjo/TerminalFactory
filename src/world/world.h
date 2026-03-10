#ifndef WORLD_H
#define WORLD_H

#include "tiles.h"

typedef struct World World;

World *createWorld();

void generateChunk(World *world, int chunkX, int chunkY);

Tile getTile(World *world, int x, int y);

void setTile(World *world, int x, int y, Tile tile);


#endif
