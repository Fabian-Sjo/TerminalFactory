#include "../src/utils/systemIndependant.h"
#include "../src/utils/perlin.h"
#include "../src/utils/map.h"
#include "../src/terminal/terminal.h"

#include "../src/graphical/renderer.h"
#include "../src/graphical/canvas.h"

#include "../src/game/player.h"
#include "../src/game/gameLoop.h"

#include "../src/world/world.h"
#include "../src/world/pathFinder.h"

#include "../src/gameData.h"

#include <assert.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>

Vector2Int screenSafezone = {0, 10};
int borderSize = 1;

// World *world = NULL;
// Canvas *canvas = NULL;

// typedef struct GameData
//{
//	KeyEvent keyevent;
//	Player *player;
//
//	int frame;
//
//	Vector2Int screenSize;
//
//	World *activeWorld;
//	Canvas *canvas;
// } GameData;
GameData gameData;
Player *player;
Vector2Int cursorPos;
Direction placeDirection = DIR_NORTH;
TileKind selectedTile = TILE_BELT;
Canvas *canvas;
World *world;

void render()
{

	writeAreaToCanvas(world, canvas, (Vector2Int){0, 0}, (Vector2Int){20, 20}, (Vector2Int){0, 0}, &gameData);

	rendererDrawCanvas(canvas);
	rendererFlush();
	// terminalDrawCanvas(canvas);
	// terminalDraw();
}
Chunk emptyChunkGenerator(Vector2Int chunkCoord)
{
	bool world[20][20] = {
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
		{1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
		{1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1},
		{1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
		{1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
		{1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
		{0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
		{1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
		{1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0},
		{1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
		{1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
		{1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	};
	Chunk chunk;
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			Vector2Int globalPos = {
				x + chunkCoord.x * CHUNK_SIZE,
				y + chunkCoord.y * CHUNK_SIZE,
			};
			Tile tile;
			tile = (Tile){.kind = TILE_ROCK, .isFunctional = false, .pos = globalPos, .sprite = {.icon = '#', .colorFore = COLOR_WHITE, .colorBack = (Color){10, 10, 10}}};
			if (!(globalPos.x < 0 || globalPos.x >= 20 || globalPos.y < 0 || globalPos.y >= 20))
				if (world[globalPos.y][globalPos.x])
					tile = (Tile){.kind = TILE_NONE, .isFunctional = false, .pos = globalPos, .sprite = {.icon = '#', .colorFore = COLOR_WHITE, .colorBack = (Color){10, 10, 10}}};
			setChunkTile(&chunk, x, y, tile);

			GroundTile groundTile = (GroundTile){.sprite = {" ", COLOR_BLACK_CONST, {0, 0, 0}}};
			setChunkGroundTile(&chunk, x, y, groundTile);
		}
	}
	return chunk;
}

/*
	  > - > - > - > - > - > - > - > - > - > - > - > - > - > - > - > - > - > - > - > -
A			  v
A			  v
A			  v
< < < < < < < <

*/

int main(int argc, char const *argv[])
{
	terminalInit();
	world = createWorld();
	canvas = canvasNew((Vector2Int){30, 30});
	int chunkGenerateRadius = 3;
	for (int x = -chunkGenerateRadius; x < chunkGenerateRadius; x++)
	{
		for (int y = -chunkGenerateRadius; y < chunkGenerateRadius; y++)
			generateChunk(world, x * CHUNK_SIZE, y * CHUNK_SIZE, &emptyChunkGenerator);
	}

	gameData.activeWorld = world;
	PathFinderPath path;
	struct timespec start, end;
	size_t count = 10000;
	timespec_get(&start, TIME_UTC);
	for (size_t i = 0; i < count; i++)
	{
		path = getPath((Vector2Int){1, 2}, (Vector2Int){19, 19}, gameData.activeWorld);
	}
	timespec_get(&end, TIME_UTC);
	double elapsed =
		(end.tv_sec - start.tv_sec) +
		(end.tv_nsec - start.tv_nsec) / 1e9;

	printf("time    %.9fs\ntimePer %.9fs", elapsed, elapsed / count);
	return;
	printf("length: %d result: %d\n", path.length, path.result);
	if (path.length > 0)
		for (size_t i = 0; i < path.length; i++)
		{
			Vector2Int point = path.points[i];
			// printf("%30spoint %2d : {%2d, %2d}\n", "", i, point.x, point.y);

			setGroundTile(
				gameData.activeWorld,
				path.points[i].x,
				path.points[i].y,
				(GroundTile){
					.sprite = (Sprite){
						.icon = '.',
						.colorFore = COLOR_WHITE}});
		}
	render();
	return 0;
}
