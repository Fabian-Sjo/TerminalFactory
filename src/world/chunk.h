#ifndef CHUNK_H
#define CHUNK_H

#include "tiles.h"
#include "groundTile.h"

#define CHUNK_SIZE 32
typedef struct
{
	Tile *tiles[CHUNK_SIZE][CHUNK_SIZE];
	GroundTile *groundTiles[CHUNK_SIZE][CHUNK_SIZE];
} Chunk;

Tile *getChunkTile(Chunk *chunk, int x, int y);
GroundTile *getChunkGroundTile(Chunk *chunk, int x, int y);

void setChunkTile(Chunk *chunk, int x, int y, Tile *tile);
void setChunkGroundTile(Chunk *chunk, int x, int y, GroundTile *groundTile);

#endif
