

#include "chunk.h"
#include "../utils/map.h"
#include "../utils/vector2.h"
#include "../utils/perlin.h"
#include <stdlib.h>
#include "tiles.h"

#define REGION_SIZE 16

typedef struct {
	Chunk chunks[REGION_SIZE][REGION_SIZE];
} Region;

typedef struct
{
	Vector2Int quadSize[4];
	Region **regionQuad[4];
} World;

void mög(World *world, Vector2Int chunckCoords) {
	int quadrum = (!chunckCoords.x)*2 + !chunckCoords.y;
	int x = (chunckCoords.x < 0) ? -chunckCoords.x : chunckCoords.x;
	int y = (chunckCoords.y < 0) ? -chunckCoords.y : chunckCoords.y;
	if (x / REGION_SIZE < world->quadSize[quadrum].x ||
			y / REGION_SIZE < world->quadSize[quadrum].y) {
		generateChunk(chunckCoords);
	}
	Chunk chunk = world->regionQuad[quadrum][x / REGION_SIZE][y / REGION_SIZE]
			.chunks[x % REGION_SIZE][y % REGION_SIZE];
	Chunk *returnChunk = &chunk;
	return returnChunk;
}

Chunk *getChunk(World *world, Vector2Int chunckCoords)
{
	if (chunckCoords.x >= 0) {
		if (chunckCoords.y >= 0) {
			if (chunckCoords.x / REGION_SIZE < world->quadSize1.x ||
					chunckCoords.y / REGION_SIZE < world->quadSize1.y) {
				generateChunk(chunckCoords);
			}
			Chunk chunk = world->regionQuad1
					[chunckCoords.x / REGION_SIZE][chunckCoords.y / REGION_SIZE]
					.chunks[chunckCoords.x % REGION_SIZE][chunckCoords.y % REGION_SIZE];	
			Chunk *returnChunk = &chunk;
			return returnChunk;
		}
		else {
			if (chunckCoords.x / REGION_SIZE < world->quadSize1.x ||
					-chunckCoords.y / REGION_SIZE < world->quadSize1.y) {
				generateChunk(chunckCoords);
			}
			Chunk chunk = world->regionQuad4
					[chunckCoords.x / REGION_SIZE][-chunckCoords.y / REGION_SIZE]
					.chunks[chunckCoords.x % REGION_SIZE][-chunckCoords.y % REGION_SIZE];	
			Chunk *returnChunk = &chunk;
			return returnChunk;
		}
	}
	else {
		if (chunckCoords.y >= 0) {
			if (-chunckCoords.x / REGION_SIZE < world->quadSize1.x ||
					chunckCoords.y / REGION_SIZE < world->quadSize1.y) {
				generateChunk(chunckCoords);
			}
			Chunk chunk = world->regionQuad2
					[-chunckCoords.x / REGION_SIZE][chunckCoords.y / REGION_SIZE]
					.chunks[-chunckCoords.x % REGION_SIZE][chunckCoords.y % REGION_SIZE];	
			Chunk *returnChunk = &chunk;
			return returnChunk;
		}
		else {
			if (-chunckCoords.x / REGION_SIZE < world->quadSize1.x ||
					-chunckCoords.y / REGION_SIZE < world->quadSize1.y) {
				generateChunk(chunckCoords);
			}
			Chunk chunk = world->regionQuad3
					[-chunckCoords.x / REGION_SIZE][-chunckCoords.y / REGION_SIZE]
					.chunks[-chunckCoords.x % REGION_SIZE][-chunckCoords.y % REGION_SIZE];	
			Chunk *returnChunk = &chunk;
			return returnChunk;
		}
	}
}

void setChunk(World *world, Vector2Int chunckCoords, Chunk *chunk)
{
	unsigned long long key = x;
	key <<= 32;
	key |= y;
	mapAdd(world->chunks, key, chunk);
}

World *createWorld()
{
	World *world = malloc(sizeof(World));
	world->chunks = mapCreate(sizeof(Map *));
	return world;
}
int chunkIsGenerated(World *world, int chunkX, int chunkY)
{
	return (getChunk(world, chunkX, chunkY) != NULL);
}

void generateChunk(World *world, int globalX, int globalY)
{
	if (globalX < 0)
		globalX -= CHUNK_SIZE - 1;
	int chunkX = globalX / CHUNK_SIZE;
	if (globalY < 0)
		globalY -= CHUNK_SIZE - 1;
	int chunkY = globalY / CHUNK_SIZE;
	if (chunkIsGenerated(world, chunkX, chunkY))
		return;

	Tile *tile = &testTile;
	GroundTile *groundTiles[] = {
		&GROUND_TILE_WATER,
		&GROUND_TILE_GRASS_1,
		&GROUND_TILE_GRASS_2,
		&GROUND_TILE_GRASS_3,
	};
	Chunk *chunk = malloc(sizeof(Chunk));
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			setChunkTile(chunk, x, y, NULL);


			float perlinValue = perlin_Get2d(
				x + chunkX * CHUNK_SIZE + 10000000,
				y + chunkY * CHUNK_SIZE + 10000000,
				0.1, 1);
			if (perlinValue < 0){
				perlinValue = 0;
			}
			if (perlinValue >= 1)
				perlinValue = 0.999999;

			int groundTileIndex = perlinValue * (sizeof(groundTiles) / sizeof(groundTiles[0]));

			setChunkGroundTile(chunk, x, y, groundTiles[groundTileIndex]);
		}
	}
	setChunk(world, chunkX, chunkY, chunk);
}
//{-1,-1}

// chunk{-1,-1}
// local{15,15}

Tile *getTile(World *world, int x, int y)
{
	int chunkLocalX = ((x % CHUNK_SIZE) + CHUNK_SIZE) % CHUNK_SIZE;
	if (x < 0)
		x -= CHUNK_SIZE - 1;
	int chunkX = x / CHUNK_SIZE;

	int chunkLocalY = ((y % CHUNK_SIZE) + CHUNK_SIZE) % CHUNK_SIZE;
	if (y < 0)
		y -= CHUNK_SIZE - 1;
	int chunkY = y / CHUNK_SIZE;

	Chunk *chunk = getChunk(world, chunkX, chunkY);
	if (chunk == NULL)
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
GroundTile *getGroundTile(World *world, int x, int y)
{
	int chunkLocalX = ((x % CHUNK_SIZE) + CHUNK_SIZE) % CHUNK_SIZE;
	if (x < 0)
		x -= CHUNK_SIZE - 1;
	int chunkX = x / CHUNK_SIZE;

	int chunkLocalY = ((y % CHUNK_SIZE) + CHUNK_SIZE) % CHUNK_SIZE;
	if (y < 0)
		y -= CHUNK_SIZE - 1;
	int chunkY = y / CHUNK_SIZE;

	Chunk *chunk = getChunk(world, chunkX, chunkY);
	if (chunk == NULL)
		return NULL;
	return getChunkGroundTile(chunk, chunkLocalX, chunkLocalY);
}

void setGroundTile(World *world, int x, int y, GroundTile *tile)
{
	int chunkX = x / CHUNK_SIZE;
	int chunkY = y / CHUNK_SIZE;
	int chunkLocalX = x - chunkX;
	int chunkLocalY = y - chunkY;
	setChunkGroundTile(getChunk(world, chunkX, chunkY), chunkLocalX, chunkLocalY, tile);
}