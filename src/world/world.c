

#include "chunk.h"
#include "../utils/map.h"
#include <stdlib.h>
#include "tiles.h"

typedef struct
{
	Map *chunks;
} World;

Chunk *getChunk(World *world, int x, int y)
{
	long key = (((long)x) << 32) + ((long)y);
	return mapGet(world->chunks, key);
}

void setChunk(World *world, int x, int y, Chunk *chunk)
{
	long key = (((long)x) << 32) + ((long)y);
	mapAdd(world->chunks, key, chunk);
}

World *createWorld()
{
	World *world = malloc(sizeof(world));
	world->chunks = mapCreate(sizeof(Map *));
	return world;
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
	setChunk(world, chunkX, chunkY, chunk);
}

Tile getTile(int *error, World *world, int x, int y)
{
	int chunkX = x / CHUNK_SIZE;
	int chunkY = y / CHUNK_SIZE;
	int chunkLocalX = x - chunkX;
	int chunkLocalY = y - chunkY;

	return getChunkTile(getChunk(world, chunkX, chunkY), chunkLocalX, chunkLocalY);
}

void setTile(World *world, int x, int y, Tile tile)
{
	int chunkX = x / CHUNK_SIZE;
	int chunkY = y / CHUNK_SIZE;
	int chunkLocalX = x - chunkX;
	int chunkLocalY = y - chunkY;
	setChunkTile(getChunk(world, chunkX, chunkY), chunkLocalX, chunkLocalY, tile);
}