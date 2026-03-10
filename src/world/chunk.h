#ifndef CHUNK_H
#define CHUNK_H

#include "tiles.h"

#define CHUNK_SIZE 16

typedef struct Chunk
{
	Tile tiles[CHUNK_SIZE][CHUNK_SIZE];
} Chunk;

Tile getChunkTile(Chunk *chunk, int x, int y);

void setChunkTile(Chunk *chunk, int x, int y, Tile tile);

#endif
