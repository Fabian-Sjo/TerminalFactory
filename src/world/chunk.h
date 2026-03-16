#ifndef CHUNK_H
#define CHUNK_H

#include "tiles.h"
#include "groundTile.h"

//Can only be power of 2 due to "x & (CHUNK_SIZE - 1)"
#define CHUNK_SIZE 8

typedef struct Chunk
{
	
	Tile tiles[CHUNK_SIZE][CHUNK_SIZE];
	GroundTile *groundTiles[CHUNK_SIZE][CHUNK_SIZE];
} Chunk;

Tile *getChunkTile(Chunk *chunk, int x, int y);
GroundTile *getChunkGroundTile(Chunk *chunk, int x, int y);

void setChunkTile(Chunk *chunk, int x, int y, Tile tile);
void setChunkGroundTile(Chunk *chunk, int x, int y, GroundTile *groundTile);

#endif
