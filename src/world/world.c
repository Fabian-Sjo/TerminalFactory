

#include "chunk.h"
#include "../utils/map.h"
#include <stdlib.h>
#include "tiles.h"

typedef struct
{
	Map *chunks;
} World;

Chunk *getChunk(World *world, unsigned int x, unsigned int y)
{
	unsigned long long key = x;
	key <<= 32;
	key |= y;
	return mapGet(world->chunks, key);
}

void setChunk(World *world, unsigned int x, unsigned int y, Chunk *chunk)
{
	unsigned long long key = x;
	key <<= 32;
	key |= y;
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
	Tile *tile = &testTile;	
	Chunk *chunk = malloc(sizeof(Chunk));
	for (size_t x = 0; x < CHUNK_SIZE; x++)
	{
		for (size_t y = 0; y < CHUNK_SIZE; y++)
		{
			setChunkTile(chunk, x, y, tile);
		}
	}
	setChunk(world, chunkX, chunkY, chunk);
}
//{-1,-1}

// chunk{-1,-1}
// local{15,15}

Tile *getTile(World *world, int x, int y)
{
	int chunkLocalX = (x + CHUNK_SIZE) % CHUNK_SIZE;
	if (x < 0)
	x -= CHUNK_SIZE-1;
	int chunkX = x / CHUNK_SIZE;

	int chunkLocalY = (y + CHUNK_SIZE) % CHUNK_SIZE;
	if (y < 0)
	y -= CHUNK_SIZE-1;
	int chunkY = y / CHUNK_SIZE;
	
	Chunk *chunk = getChunk(world, chunkX, chunkY);
	if(chunk == NULL)
		return NULL;
	return getChunkTile(chunk, chunkLocalX, chunkLocalY);
}

void setTile(World *world, int x, int y, Tile *tile)
{
	int chunkX = x / CHUNK_SIZE;
	int chunkY = y / CHUNK_SIZE;
	int chunkLocalX = x - chunkX;
	int chunkLocalY = y - chunkY;
	setChunkTile(getChunk(world, chunkX, chunkY), chunkLocalX, chunkLocalY, tile);
}