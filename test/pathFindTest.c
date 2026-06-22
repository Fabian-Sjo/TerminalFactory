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
#include "../src/settings.h"

#include <assert.h>
#include <stdio.h>
#include <signal.h>

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

void redrawCanvasAndGui(GameData *gameData)
{
	printf("\033[2J\033[H");
	gameData->canvas = canvasNew(gameData->screenSize);
	Vector2Int screenSize = gameData->screenSize;
	NineRect nineRect = {
		{{(Sprite){'\\'}, (Sprite){'|'}, (Sprite){'/'}},
		 {(Sprite){'='}, (Sprite){'.'}, (Sprite){'='}},
		 {(Sprite){'/'}, (Sprite){'|'}, (Sprite){'\\'}}}};
	for (int i = 0; i < borderSize; i++)
	{
		canvasDrawNineRect(gameData->canvas, (Vector2Int){i, i}, (Vector2Int){screenSize.x - i * 2, screenSize.y - i * 2}, nineRect, FILL_NONE);
	}
	// canvasDrawNineRect(canvas, (Vector2Int){10, 10}, (Vector2Int){3,3}, nineRect, FILL_NONE);
	// canvasDrawNineRect(canvas, (Vector2Int){13, 10}, (Vector2Int){5,3}, nineRect, FILL_NONE);
	// canvasDrawNineRect(canvas, (Vector2Int){10, 14}, (Vector2Int){7,7}, nineRect, FILL_NONE);
	//
	//
	// cavasDrawRectangle(canvas, (Vector2Int){2, 2}, (Vector2Int){5, 5}, (Sprite){'#'}, FILL_ALL);
	// cavasDrawRectangle(canvas, (Vector2Int){8, 2}, (Vector2Int){5, 5}, (Sprite){'+'}, FILL_NONE);
	// cavasDrawRectangle(canvas, (Vector2Int){20, 20}, (Vector2Int){5, 5}, (Sprite){'#'}, FILL_ALL);
}

Vector2Int screenToWorld(Vector2Int screen)
{

	return vecAddI((Vector2Int){-1, -1}, vecAddI(vecSubI(screen, vecDivI(gameData.screenSize, (Vector2Int){2, 2})), (Vector2Int){0, 0}));
}
void render(double deltaTime, GameData *gameData)
{
	Vector2Int termsize = getTermSize();
	termsize = vecDivI(vecSubI(termsize, screenSafezone), (Vector2Int){1 + settingDoHorisontalSpacing, 1});
	if (termsize.x != gameData->screenSize.x || termsize.y != gameData->screenSize.y)
	{
		gameData->screenSize = termsize;
		redrawCanvasAndGui(gameData);
	}
	Vector2Int actualScreenSize = vecSubI(gameData->screenSize, screenSafezone);
	writeAreaToCanvas(gameData->activeWorld, gameData->canvas, (Vector2Int){-17, -17}, (Vector2Int){gameData->screenSize.x - borderSize * 2, gameData->screenSize.y - borderSize * 2}, (Vector2Int){borderSize, borderSize}, gameData);

	terminalSetCursorPos((Vector2Int){0, 0});
	rendererDrawCanvas(gameData->canvas);
	rendererFlush();
	// terminalDrawCanvas(gameData->canvas);
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

			GroundTile groundTile = (GroundTile){.sprite = {' ', COLOR_BLACK_CONST, {0, 0, 0}}};
			setChunkGroundTile(&chunk, x, y, groundTile);
		}
	}
	return chunk;
}
void gameInit()
{
	// has to be power of 2
	// assert(CHUNK_SIZE && !(CHUNK_SIZE & (CHUNK_SIZE - 1)));

	// signal(SIGINT, stopGame);
	terminalInit();
	World *world = createWorld();
	gameData.activeWorld = world;
	gameData.canvas = canvasNew((Vector2Int){30, 30});
	int chunkGenerateRadius = 3;
	for (int x = -chunkGenerateRadius; x < chunkGenerateRadius; x++)
	{
		for (int y = -chunkGenerateRadius; y < chunkGenerateRadius; y++)
			generateChunk(world, x * CHUNK_SIZE, y * CHUNK_SIZE, &emptyChunkGenerator);
	}
	render(1, &gameData);
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
	gameInit();

	Path path = getPath((Vector2Int){1, 2}, (Vector2Int){19, 19}, gameData.activeWorld);
	printf("length: %d result: %d\n", path.length, path.result);
	if (path.length > 0)
		for (size_t i = 0; i < path.length; i++)
		{
			Vector2Int point = path.points[i];
			printf("point %d : {%d, %d}\n", i, point.x, point.y);

			setGroundTile(
				gameData.activeWorld,
				path.points[i].x,
				path.points[i].y,
				(GroundTile){
					.sprite = (Sprite){
						.icon = '.',
						.colorFore = COLOR_WHITE}});
			render(1, &gameData);
		}

	return 0;
}
