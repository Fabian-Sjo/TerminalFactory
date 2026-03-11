#ifndef WORLD_H
#define WORLD_H

#include "tiles.h"
#include "groundTile.h"

typedef struct World World;

World *createWorld();
int chunkIsGenerated(World *world, int x, int y);
void generateChunk(World *world, int x, int y);

Tile *getTile(World *world, int x, int y);

GroundTile *getGroundTile(World *world, int x, int y);

void setTile(World *world, int x, int y, Tile tile);

void setGroundTile(World *world, int x, int y, GroundTile tile);


#endif
