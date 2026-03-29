#include "../utils/perlin.h"
#include "chunkGenerator.h"

Chunk generateMoonChunk(Vector2Int chunkCoordinate)
{

	// Tile *tile = &testTile;
	GroundTile groundTiles[] = {
		{.sprite = {'^', COLOR_BLACK_CONST, {0, 0, 0}}},
		{.sprite = {'.', COLOR_BLACK_CONST, {0, 0, 0}}},
		{.sprite = {'_', COLOR_BLACK_CONST, {0, 0, 0}}},
		{.sprite = {'-', COLOR_BLACK_CONST, {0, 0, 0}}}};
	Chunk chunk;
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			// TODO this no work
			setChunkTile(&chunk, x, y, (Tile){0});

			float perlinValue = perlin_Get2d(
				x + chunkCoordinate.x * CHUNK_SIZE + 10000000,
				y + chunkCoordinate.y * CHUNK_SIZE + 10000000,
				0.1, 1);
			if (perlinValue < 0)
			{
				perlinValue = 0;
			}
			if (perlinValue >= 1)
				perlinValue = 0.9999f;

			if (perlinValue < 0.3)
				perlinValue = 0;
			else if (perlinValue < 0.6)
				perlinValue = 0.5;
			else
				perlinValue = 0.9999f;
			int groundTileIndex = perlinValue * (sizeof(groundTiles) / sizeof(groundTiles[0]));

			setChunkGroundTile(&chunk, x, y, groundTiles[groundTileIndex]);
		}
	}
	return chunk;
}

Tile generateTile(Vector2Int globalPos);

GroundTile generateGroundTile(Vector2Int globalPosition);
