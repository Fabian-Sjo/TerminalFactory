#ifndef CHUNK_H
#define CHUNK_H

#include "tiles.h"
#include "groundTile.h"

//Can only be power of 2 due to "x & (CHUNK_SIZE - 1)"
#define CHUNK_SIZE 8

typedef struct Chunk
{
	
	Tile tiles[CHUNK_SIZE][CHUNK_SIZE];
	GroundTile groundTiles[CHUNK_SIZE][CHUNK_SIZE];
} Chunk;

// Returns the tile stored at the requested chunk-local coordinates.
Tile *getChunkTile(Chunk *chunk, int x, int y);
// Returns the ground tile stored at the requested chunk-local coordinates.
GroundTile *getChunkGroundTile(Chunk *chunk, int x, int y);

// Writes a tile into the chunk at the requested chunk-local coordinates.
void setChunkTile(Chunk *chunk, int x, int y, Tile tile);
// Writes a ground tile into the chunk at the requested chunk-local coordinates.
void setChunkGroundTile(Chunk *chunk, int x, int y, GroundTile groundTile);

#endif
