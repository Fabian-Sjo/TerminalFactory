

#include "chunk.h"
#include "../utils/map.h"
#include <stdlib.h>
#include "tiles.h"

typedef struct
{
	Chunk *chunks[3][3];
} World;

World *createWorld()
{
	return malloc(sizeof(World));
}

void generateChunk(World *world, int chunkX, int chunkY)
{
	Chunk *chunk = malloc(sizeof(Chunk));
	for (size_t x = 0; x < CHUNK_SIZE; x++)
	{
		for (size_t y = 0; y < CHUNK_SIZE; y++)
		{
			setChunkTile(chunk, x, y, testTile);
		}
	}
	world->chunks[chunkX][chunkY] = chunk;
}

Tile getTile(World *world, int x, int y)
{
}

void setTile(World *world, int x, int y, Tile tile)
{
}