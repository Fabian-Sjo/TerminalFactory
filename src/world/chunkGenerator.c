#include "../utils/perlin.h"
#include "chunkGenerator.h"
#include <math.h>

Vector2Float sobelFilter(double (*getPointValue)(Vector2Int pos), Vector2Int pos)
{
	double values[3][3];
	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			values[y + 1][x + 1] = getPointValue(vecAddI(pos, (Vector2Int){x, y}));
		}
	}
	double kernelX[3][3] = {
		{-1, 0, 1},
		{-2, 0, 2},
		{-1, 0, 1}};
	double sumX = 0;
	for (int i = 0; i < 9; i++)
	{
		sumX += values[i / 3][i % 3] * kernelX[i / 3][i % 3];
	}

	double kernelY[3][3] = {
		{-1, -2, -1},
		{0, 0, 0},
		{1, 2, 1}};
	double sumY = 0;
	for (int i = 0; i < 9; i++)
	{
		sumY += values[i / 3][i % 3] * kernelY[i / 3][i % 3];
	}
	return (Vector2Float){sumX, sumY};
}
double getPerlinValue(Vector2Int pos)
{
	// return (abs(pos.x) + abs(pos.y)) > 11;
	return perlin_Get2d(pos.x + 1000, pos.y + 1000, 0.05, 1);
}
Chunk generateMoonChunk(Vector2Int chunkCoordinate)
{

	// Tile *tile = &testTile;

	Chunk chunk;
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			// TODO this no work
			Vector2Int globalPos = {
				x + chunkCoordinate.x * CHUNK_SIZE,
				y + chunkCoordinate.y * CHUNK_SIZE,
			};
			setChunkTile(&chunk, x, y, (Tile){0});

			double perlinValue = getPerlinValue(globalPos);

			int random = ((int)(perlinValue * 100) ^ (int)(perlinValue * 1000)) % 100;
			
			Vector2Float normal = sobelFilter(&getPerlinValue, globalPos);
			double direction = atan2(normal.y, normal.x);
			direction = ((direction / 3.14159) + 1) / 2; // 0 to 1
			
			Tile tile = (Tile){.kind = TILE_NONE, .isFunctional = false, .pos = globalPos};
			double magnitude = sqrt(normal.x * normal.x + normal.y * normal.y);
			if(magnitude < 0.06)
			{
				tile.kind = TILE_ROCK;
				tile.sprite.icon = 'O';
			}
			else if (magnitude < 0.1)
			{
				tile.kind = TILE_ROCK;
				if (direction < 0.1)
				tile.sprite.icon = '|';
				else if (direction < 0.125)
				tile.sprite.icon = '/';
				else if (direction < 0.325)
				tile.sprite.icon = '-';
				else if (direction < 0.425)
				tile.sprite.icon = '\\';
				else if (direction < 0.625)
				tile.sprite.icon = '|';
				else if (direction < 0.675)
				tile.sprite.icon = '/';
				else if (direction < 0.825)
				tile.sprite.icon = '-';
				else if (direction < 0.925)
				tile.sprite.icon = '\\';
				else
				tile.sprite.icon = '|';
			}
			int colorValue = fabs(getPerlinValue(globalPos)) * 100;
			if (colorValue > 254)
			colorValue = 254;
			
			setChunkTile(&chunk, x, y, tile);
			
			GroundTile groundTile = (GroundTile){.sprite = {' ', COLOR_BLACK_CONST, {0, 0, 0}}};
			if(random < 5)
			{
				groundTile.sprite.icon = '.';
			}
			setChunkGroundTile(&chunk, x, y, groundTile);
		}
	}
	return chunk;
}

Tile generateTile(Vector2Int globalPos);

GroundTile generateGroundTile(Vector2Int globalPosition);
