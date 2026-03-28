

#include "chunk.h"
#include "tiles.h"
#include "world.h"
#include "tileHandler.h"

#include "../gameData.h"

#include "../utils/perlin.h"
#include "../utils/vector2.h"

#include "../graphical/renderer.h"
#include "../graphical/canvas.h"

#include "../utils/map.h"

#include <stdlib.h>
#include <stdio.h>

#define true 1
#define false 0
struct World
{
	Map *chunks;
	TileHandler tileHandler;
	int tilesMax;
	int tilesCurrent;
	void *Tiles;
};
void worldTick(GameData *gameData)
{

	TileHandler *handler = &(gameData->activeWorld->tileHandler);
	tickFunctionTiles(handler, gameData);
}
void *worldTileFromInstanceID(World *world, int instanceID)
{
	return world->tileHandler.instances[instanceID].data;
}
Vector2Int worldTileOriginPosFromId(World *world, int instanceID)
{
	TileHandler *handler = &(world->tileHandler);
	if (instanceID < 0 || instanceID >= handler->count)
		return (Vector2Int){0, 0};
	TileKind kind = handler->instances[instanceID].kind;
	if (kind == TILE_NONE)
		return (Vector2Int){0, 0};
	TileDefinition *def = getTileDefinition(kind);
	if (def == NULL)
		return (Vector2Int){0, 0};

	return handler->instances[instanceID].pos;
}
int nrOfChunks(World *world)
{
	return mapGetSize(world->chunks);
};

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
	World *world = malloc(sizeof(World));
	world->chunks = mapCreate(sizeof(Map *));
	for (size_t i = 0; i < MAX_FUNCTION_TILES; i++)
	{
		world->tileHandler.instances[i].kind = TILE_NONE;
		world->tileHandler.instances[i].data = NULL;
	}

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

	// Tile *tile = &testTile;
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
			// TODO this no work
			setChunkTile(chunk, x, y, createFunctionTile(&world->tileHandler, TILE_NONE, (Vector2Int){x, y}, NULL));

			float perlinValue = perlin_Get2d(
				x + chunkX * CHUNK_SIZE + 10000000,
				y + chunkY * CHUNK_SIZE + 10000000,
				0.1, 1);
			if (perlinValue < 0)
			{
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

int divFloor(int a, int b)
{
	int q = a / b;
	int r = a % b;

	if ((r != 0) && ((r < 0) != (b < 0)))
		q--;

	return q;
}

void writeAreaToCanvas(World *world, Canvas *canvas, Vector2Int posA, Vector2Int size, Vector2Int canvasPos, GameData *gameData)
{
	Vector2Int posB = vecAddI(posA, size);
	int width = posB.x - posA.x;
	int height = posB.y - posA.y;
	int required = width * height;

	Vector2Int minChunk = {
		divFloor(posA.x, CHUNK_SIZE),
		divFloor(posA.y, CHUNK_SIZE)};

	Vector2Int maxChunk = {
		divFloor(posB.x, CHUNK_SIZE),
		divFloor(posB.y, CHUNK_SIZE)};

	for (int chunkX = minChunk.x; chunkX <= maxChunk.x; chunkX++)
	{
		for (int chunkY = minChunk.y; chunkY <= maxChunk.y; chunkY++)
		{
			Chunk *chunk = getChunk(world, chunkX, chunkY);

			int startX = posA.x - chunkX * CHUNK_SIZE;
			int endX = posB.x - chunkX * CHUNK_SIZE;

			int startY = posA.y - chunkY * CHUNK_SIZE;
			int endY = posB.y - chunkY * CHUNK_SIZE;

			if (startX < 0)
				startX = 0;
			if (startY < 0)
				startY = 0;
			if (endX > CHUNK_SIZE)
				endX = CHUNK_SIZE;
			if (endY > CHUNK_SIZE)
				endY = CHUNK_SIZE;

			int chunkWorldX = chunkX * CHUNK_SIZE;
			int chunkWorldY = chunkY * CHUNK_SIZE;

			for (int localX = startX; localX < endX; localX++)
			{
				for (int localY = startY; localY < endY; localY++)
				{
					int globalX = chunkWorldX + localX;
					int globalY = chunkWorldY + localY;

					Sprite sprite = {' ', COLOR_BLACK};

					if (chunk)
					{
						GroundTile *groundTile = getChunkGroundTile(chunk, localX, localY);
						sprite = getGroundTileSprite(groundTile);

						Tile *tile = getChunkTile(chunk, localX, localY);
						if (tile->kind != TILE_NONE)
						{
							TileDefinition *def = getTileDefinition(tile->kind);
							if (!def)
								continue;
							Sprite tileSprite = def->getSprite(tile->instanceID, (Vector2Int){globalX, globalY}, gameData);
							// Sprite tileSprite = tileGetSprite(tile);
							sprite.icon = tileSprite.icon;
							sprite.colorFore = tileSprite.colorFore;
						}
					}

					int screenX = globalX - posA.x;
					int screenY = globalY - posA.y;

					int canvasX = canvasPos.x + screenX;
					int canvasY = canvasPos.y + screenY;

					if (canvasX < 0 || canvasY < 0 ||
						canvasX >= canvasGetSize(canvas).x ||
						canvasY >= canvasGetSize(canvas).y)
						continue;
					canvasSetSprite(canvas, (Vector2Int){canvasX, canvasY}, sprite);
					// canvas->sprites[canvasX + canvasY * canvas->size.x] = sprite;
				}
			}
		}
	}
}

Tile *getTile(World *world, int x, int y)
{
	int chunkLocalX = x & (CHUNK_SIZE - 1);
	if (x < 0)
		x -= CHUNK_SIZE - 1;
	int chunkX = x / CHUNK_SIZE;

	int chunkLocalY = y & (CHUNK_SIZE - 1);
	if (y < 0)
		y -= CHUNK_SIZE - 1;
	int chunkY = y / CHUNK_SIZE;

	Chunk *chunk = getChunk(world, chunkX, chunkY);
	if (chunk == NULL)
		return NULL;
	return getChunkTile(chunk, chunkLocalX, chunkLocalY);
}
int canPlaceTile(World *world, Vector2Int position, TileKind tileKind)
{
	TileDefinition *def = getTileDefinition(tileKind);
	if (def == NULL)
		return false;
	Tile *tileAtPos = getTile(world, position.x, position.y);
	if (tileAtPos == NULL || tileAtPos->kind != TILE_NONE)
		return false;
	return true;
}
int canPlaceMultiTile(World *world, Vector2Int originPosition, TileKind tileKind)
{
	Vector2Int tileSize = getTileSize(tileKind);
	originPosition = vecSubI(originPosition, getTileOriginOffset(tileKind));
	for (int x = 0; x < tileSize.x; x++)
	{
		for (int y = 0; y < tileSize.y; y++)
		{
			Vector2Int position = vecAddI(originPosition, (Vector2Int){x, y});
			Tile *tileAtPos = getTile(world, position.x, position.y);
			if (tileAtPos == NULL || tileAtPos->kind != TILE_NONE)
				return false;
		}
	}
	return true;
}
int placeTile(World *world, Vector2Int originPosition, TileKind tileKind)
{

	Vector2Int tileSize = getTileSize(tileKind);
	if (canPlaceMultiTile(world, originPosition, tileKind) == false)
		return -1;
	originPosition = vecSubI(originPosition, getTileOriginOffset(tileKind));
	int originID = -1;
	for (int x = 0; x < tileSize.x; x++)
	{
		for (int y = 0; y < tileSize.y; y++)
		{
			Vector2Int position = vecAddI(originPosition, (Vector2Int){x, y});
			int chunkLocalX = position.x & (CHUNK_SIZE - 1);
			if (position.x < 0)
				position.x -= CHUNK_SIZE - 1;
			int chunkX = position.x / CHUNK_SIZE;

			int chunkLocalY = position.y & (CHUNK_SIZE - 1);
			if (position.y < 0)
				position.y -= CHUNK_SIZE - 1;
			int chunkY = position.y / CHUNK_SIZE;

			destroyFunctionTile(&world->tileHandler, getChunkTile(getChunk(world, chunkX, chunkY), chunkLocalX, chunkLocalY)->instanceID);
			Tile tile;
			if (x == 0 && y == 0)
			{
				tile = createFunctionTile(&world->tileHandler, tileKind, originPosition, NULL);
				originID = tile.instanceID;
			}
			else
			{
				tile = createMultiTile(&world->tileHandler, tileKind, (Vector2Int){x, y}, originID);
			}

			setChunkTile(getChunk(world, chunkX, chunkY), chunkLocalX, chunkLocalY, tile);
		}
	}
	return 0;
}
void removeTile(World *world, Vector2Int deletePosition)
{

	Tile *tile = getTile(world, deletePosition.x, deletePosition.y);
	Vector2Int originPosition = worldTileOriginPosFromId(world, tile->instanceID);
	if (tile == NULL || tile->kind == TILE_NONE)
		return;
	TileDefinition *def = getTileDefinition(tile->kind);
	Vector2Int tileSize = def->size;
	destroyFunctionTile(&world->tileHandler, tile->instanceID);
	for (int x = 0; x < tileSize.x; x++)
	{
		for (int y = 0; y < tileSize.y; y++)
		{
			Vector2Int position = vecAddI(originPosition, (Vector2Int){x, y});
			int chunkLocalX = position.x & (CHUNK_SIZE - 1);
			if (position.x < 0)
				position.x -= CHUNK_SIZE - 1;
			int chunkX = position.x / CHUNK_SIZE;

			int chunkLocalY = position.y & (CHUNK_SIZE - 1);
			if (position.y < 0)
				position.y -= CHUNK_SIZE - 1;
			int chunkY = position.y / CHUNK_SIZE;
			Tile noTile = {
				.kind = TILE_NONE,
				.instanceID = -1,
			};
			setChunkTile(getChunk(world, chunkX, chunkY), chunkLocalX, chunkLocalY, noTile);
		}
	}
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