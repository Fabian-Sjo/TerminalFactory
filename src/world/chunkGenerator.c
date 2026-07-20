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
			if (abs(globalPos.x) + abs(globalPos.y) < 10)
				magnitude++;

			if (magnitude < 0.06)
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
			if (random < 5)
			{
				groundTile.sprite.icon = '.';
			}
			setChunkGroundTile(&chunk, x, y, groundTile);
		}
	}
	return chunk;
}
Chunk generateCraterChunk(Vector2Int chunkCoordinate)
{
    Chunk chunk;

    for (int x = 0; x < CHUNK_SIZE; x++)
    {
        for (int y = 0; y < CHUNK_SIZE; y++)
        {
            Vector2Int globalPos = {
                x + chunkCoordinate.x * CHUNK_SIZE,
                y + chunkCoordinate.y * CHUNK_SIZE};

            Tile tile = {
                .kind = TILE_NONE,
                .isFunctional = false,
                .pos = globalPos};

            GroundTile ground = {
                .sprite = {' ', COLOR_BLACK_CONST, {0, 0, 0}}};

            // Large-scale terrain
            double terrain = perlin_Get2d(globalPos.x, globalPos.y, 0.012, 3);

            // Crater field
            double crater = 1000.0;

            int cellSize = 24;

            int cellX = floor((double)globalPos.x / cellSize);
            int cellY = floor((double)globalPos.y / cellSize);

            for (int ox = -1; ox <= 1; ox++)
            {
                for (int oy = -1; oy <= 1; oy++)
                {
                    int gx = cellX + ox;
                    int gy = cellY + oy;

                    double rx = perlin_Get2d(gx + 1000, gy + 1000, 1.0, 1);
                    double ry = perlin_Get2d(gx + 5000, gy + 5000, 1.0, 1);

                    int cx = gx * cellSize + (int)((rx + 1.0) * 0.5 * cellSize);
                    int cy = gy * cellSize + (int)((ry + 1.0) * 0.5 * cellSize);

                    double dx = globalPos.x - cx;
                    double dy = globalPos.y - cy;

                    double dist = sqrt(dx * dx + dy * dy);

                    if (dist < crater)
                        crater = dist;
                }
            }

            double height = terrain;

            // Bowl
            if (crater < 8)
                height -= (8 - crater) * 0.12;

            // Raised rim
            if (crater > 8 && crater < 11)
                height += (11 - crater) * 0.15;

            if (height > 0.45)
            {
                tile.kind = TILE_ROCK;
                tile.sprite.icon = '^';
            }
            else if (height > 0.25)
            {
                tile.kind = TILE_ROCK;
                tile.sprite.icon = '#';
            }
            else if (height > 0.0)
            {
                tile.kind = TILE_ROCK;
                tile.sprite.icon = 'O';
            }
            else if (height > -0.45)
            {
                tile.kind = TILE_NONE;
                ground.sprite.icon = '.';
            }
            else
            {
                tile.kind = TILE_NONE;
                ground.sprite.icon = ',';
            }

            // Random boulders
            double rockNoise = perlin_Get2d(globalPos.x + 2000,
                                            globalPos.y + 2000,
                                            0.25,
                                            1);

            if (tile.kind == TILE_NONE && rockNoise > 0.72)
            {
                tile.kind = TILE_ROCK;
                tile.sprite.icon = 'o';
            }

            // Small crater pits
            double pitNoise = perlin_Get2d(globalPos.x + 8000,
                                           globalPos.y + 8000,
                                           0.08,
                                           2);

            if (tile.kind == TILE_NONE && pitNoise < -0.78)
            {
                ground.sprite.icon = '*';
            }

            setChunkTile(&chunk, x, y, tile);
            setChunkGroundTile(&chunk, x, y, ground);
        }
    }

    return chunk;
}
Chunk generateCrystalCaveChunk(Vector2Int chunkCoordinate)
{
    Chunk chunk;

    for (int x = 0; x < CHUNK_SIZE; x++)
    {
        for (int y = 0; y < CHUNK_SIZE; y++)
        {
            Vector2Int globalPos = {
                x + chunkCoordinate.x * CHUNK_SIZE,
                y + chunkCoordinate.y * CHUNK_SIZE};

            Tile tile = {
                .kind = TILE_NONE,
                .isFunctional = false,
                .pos = globalPos};

            GroundTile ground = {
                .sprite = {' ', COLOR_BLACK_CONST, {0, 0, 0}}};


            /*
                Base cave shape.

                Low frequency creates large cave chambers.
                Higher frequency adds detail.
            */
            double large =
                perlin_Get2d(globalPos.x + 4000,
                             globalPos.y + 4000,
                             0.025,
                             2);

            double detail =
                perlin_Get2d(globalPos.x - 7000,
                             globalPos.y + 7000,
                             0.12,
                             1);


            double caveValue = large * 0.8 + detail * 0.2;


            /*
                Rock walls
            */
            if (caveValue > 0.05)
            {
                tile.kind = TILE_ROCK;

                // Make walls directional-looking
                Vector2Float normal = sobelFilter(&getPerlinValue, globalPos);

                double angle = atan2(normal.y, normal.x);

                if (angle < -2.35)
                    tile.sprite.icon = '/';
                else if (angle < -0.8)
                    tile.sprite.icon = '\\';
                else if (angle < 0.8)
                    tile.sprite.icon = '|';
                else
                    tile.sprite.icon = '-';
            }
            else
            {
                /*
                    Cave floor
                */
                ground.sprite.icon = '.';


                /*
                    Crystal clusters.

                    A second noise layer creates patches,
                    not random single tiles.
                */
                double crystal =
                    perlin_Get2d(globalPos.x + 12000,
                                 globalPos.y - 3000,
                                 0.08,
                                 2);


                if (crystal > 0.55)
                {
                    ground.sprite.icon = '*';
                }
            }


            /*
                Crystal pillars inside caves
            */
            double pillar =
                perlin_Get2d(globalPos.x - 9000,
                             globalPos.y + 9000,
                             0.18,
                             1);


            if (tile.kind == TILE_NONE && pillar > 0.82)
            {
                tile.kind = TILE_ROCK;
                tile.sprite.icon = '+';
            }


            /*
                Rare giant crystals.

                Hash based on coordinates, works with negatives.
            */
            int hx = globalPos.x;
            int hy = globalPos.y;

            unsigned int hash =
                (unsigned int)(hx * 374761393 +
                               hy * 668265263);

            hash = (hash ^ (hash >> 13)) * 1274126177;

            double random =
                (hash & 0xffff) / 65535.0;


            if (tile.kind == TILE_NONE && random > 0.995)
            {
                tile.kind = TILE_ROCK;
                tile.sprite.icon = 'A';
            }


            setChunkTile(&chunk, x, y, tile);
            setChunkGroundTile(&chunk, x, y, ground);
        }
    }

    return chunk;
}
Tile generateTile(Vector2Int globalPos);

GroundTile generateGroundTile(Vector2Int globalPosition);
