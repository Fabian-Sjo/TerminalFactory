#include "utils/systemIndependant.h"
#include "utils/perlin.h"
#include "utils/map.h"

#include "terminal/terminal.h"

#include "graphical/renderer.h"
#include "graphical/canvas.h"

#include "game/player.h"
#include "game/gameLoop.h"

#include "world/world.h"

#include "gameData.h"
#include "settings.h"

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

void render(double deltaTime, GameData *gameData);
void tickPlayer(double deltaTime, GameData gameData);

void debugInfo(double deltaTime, GameData *gameData)
{

	// printf("\033[2K"); // clear line
	printf("selected item[%d][%c]: %s        \n", selectedTile, getTileDefinition(selectedTile)->icon, getTileDefinition(selectedTile)->name);
	printf("screen width: %d height:%d\n", gameData->screenSize.x, gameData->screenSize.y);
	printf("delta time: %10fs FPS: %3f\n nrOfChunks: %d\n", deltaTime, (1.0 / deltaTime), nrOfChunks(gameData->activeWorld));
	printf("Frame: %d  ", gameData->frame++);
	printf("Pos x: %-3f Pos y: %-3f", player->position.x, player->position.y);
	printf("placeDir %d", placeDirection);
	// KeyEvent keyEvent = gameData->keyevent;
	// printf("pressed: %-10s released: %-10s held: %-10s \n", keyToString(keyEvent.pressed), keyToString(keyEvent.released), keyToString(keyEvent.held));
}
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
void loop(double deltaTime)
{
	// gameData.keyevent = getKeyEvent();

	gameData.frame++;
	gameData.tick++;
	tickPlayer(deltaTime, gameData);

	worldTick(&gameData);
	// debugInfo(deltaTime, keyEvent);

	debugInfo(deltaTime, &gameData);
	render(deltaTime, &gameData);

	int chunkGenerateRadius = 3;
	for (int x = -chunkGenerateRadius; x < chunkGenerateRadius; x++)
	{
		for (int y = -chunkGenerateRadius; y < chunkGenerateRadius; y++)
			generateChunk(gameData.activeWorld, player->position.x + x * 8, player->position.y + y * 8);
	}
}
Vector2Int screenToWorld(Vector2Int screen)
{

	return vecAddI((Vector2Int){-1, -1}, vecAddI(vecSubI(screen, vecDivI(gameData.screenSize, (Vector2Int){2, 2})), vecRound(player->position)));
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
	Vector2Int position = {
		player->position.x - gameData->screenSize.x / 2,
		player->position.y - gameData->screenSize.y / 2};

	writeAreaToCanvas(gameData->activeWorld, gameData->canvas, position, (Vector2Int){gameData->screenSize.x - borderSize * 2, gameData->screenSize.y - borderSize * 2}, (Vector2Int){borderSize, borderSize}, gameData);

	/*for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			Vector2Int playerScreenPos = vecAddI(vecDivI(gameData->screenSize, (Vector2Int){2, 2}), (Vector2Int){x, y});

			canvasSetSprite(gameData->canvas, playerScreenPos, (Sprite){'@', COLOR_RED_CONST, COLOR_BLACK_CONST});
		}
	}*/
	// TODO position desyncs from tileplacement and idk why
	Vector2Int previewSize = getTileSize(selectedTile);
	Vector2Int previewOriginOffset = getTileOriginOffset(selectedTile);
	Vector2Int buildPos = screenToWorld(cursorPos);
	for (int x = 0; x < previewSize.x; x++)
	{
		for (int y = 0; y < previewSize.y; y++)
		{
			Vector2Int previewPos = vecAddI(cursorPos, (Vector2Int){x, y});
			previewPos = vecSubI(previewPos, previewOriginOffset);
			if (previewPos.x < gameData->screenSize.x && previewPos.y < gameData->screenSize.y)
			{
				TileDefinition *def = getTileDefinition(selectedTile);
				Sprite sprite = (Sprite){def->icon, COLOR_WHITE, COLOR_BLACK};
				if (def->getSprite != NULL)
					sprite = def->getSprite(placeDirection, (Vector2Int){x, y}, gameData);
				if (!canPlaceTile(gameData->activeWorld, vecAddI(buildPos, vecSubI((Vector2Int){x, y}, previewOriginOffset)), selectedTile))
				{
					sprite.colorFore = (Color){100, 0, 0};
				}
				else
				{
					sprite.colorFore = (Color){0, 30, 0};
				}
				sprite.colorBack = COLOR_TRANSPARENT;
				canvasSetSprite(gameData->canvas, previewPos, sprite);
			}
		}
	}

	terminalSetCursorPos((Vector2Int){0, 0});
	rendererDrawCanvas(gameData->canvas);
	rendererFlush();
	// terminalDrawCanvas(gameData->canvas);
	// terminalDraw();
}
void tickPlayer(double deltaTime, GameData gameData)
{

	poolInput();
	cursorPos = vecDivI(terminalGetMousePos(), (Vector2Int){1 + settingDoHorisontalSpacing, 1});
	if (terminalGetKeyState(KEY_ESC) == KEY_JUST_PRESSED)
	{
		stopGame();
	};
	if (terminalGetKeyState(KEY_Q) == KEY_JUST_PRESSED)
	{
		selectedTile--;
		if (selectedTile < 0)
			selectedTile = TILE_COUNT - 1;
	};
	if (terminalGetKeyState(KEY_E) == KEY_JUST_PRESSED)
	{
		selectedTile++;
		if (selectedTile >= TILE_COUNT)
			selectedTile = 0;
	};
	if (terminalGetKeyState(KEY_R) == KEY_JUST_PRESSED)
	{
		placeDirection--;
		if (placeDirection < DIR_WEST)
			placeDirection = DIR_NORTH;
	};
	printf("sState %d", terminalGetKeyState(KEY_S));
	if (terminalGetKeyState(KEY_S))
	{
		player->position.y += deltaTime * player->speed;
	};
	if (terminalGetKeyState(KEY_W))
	{
		player->position.y -= deltaTime * player->speed;
	};
	if (terminalGetKeyState(KEY_A))
	{
		player->position.x -= deltaTime * player->speed;
	};
	if (terminalGetKeyState(KEY_D))
	{
		player->position.x += deltaTime * player->speed;
	};
	if (terminalGetKeyState(KEY_C) == KEY_JUST_PRESSED)
	{
		settingDoHorisontalSpacing = !settingDoHorisontalSpacing;
	};
	if (terminalGetKeyState(KEY_SPACE))
	{
		removeTile(gameData.activeWorld, screenToWorld(cursorPos));
	};
	if (terminalGetKeyState(KEY_MOUSE_1))
	{
		placeTile(gameData.activeWorld, screenToWorld(cursorPos), placeDirection, selectedTile);
	};
}
void start()
{
	printf("\033[2J");	// clear terminal
	printf("\33[?25l"); // reset ansi
	player = playerNew();
	World *world = createWorld();
	gameData.activeWorld = world;
	gameData.canvas = canvasNew((Vector2Int){30, 30});
	int chunkGenerateRadius = 3;
	for (int x = -chunkGenerateRadius; x < chunkGenerateRadius; x++)
	{
		for (int y = -chunkGenerateRadius; y < chunkGenerateRadius; y++)
			generateChunk(world, x * 8, y * 8);
	}

	// canvasDrawNineRect(canvas, (Vector2Int){20, 2}, (Vector2Int){10, 10}, nineRect, FILL_NONE);

	// canvasDrawNineRect(canvas, (Vector2Int){24, 6}, (Vector2Int){10, 10}, nineRect, FILL_NONE);
	//  areaAsSprites(world, (Vector2Int){1, 1}, (Vector2Int){3, 3});

	// areaAsSprites(world, (Vector2Int){CHUNK_SIZE + 1, 1}, (Vector2Int){CHUNK_SIZE + 3, 3});

	// generateChunk(world, 1, 1);
}
void stop()
{
	// TODO ansi doesnt reset, leaves colors
	printf("\033[0");
	printf("\33[?25l"); // reset ansi
	printf("\033[2J");	// clear terminal
	printf("\033[H");	// scroll back terminal
	fflush(stdout);
}

int main()
{

	// has to be power of 2
	// assert(CHUNK_SIZE && !(CHUNK_SIZE & (CHUNK_SIZE - 1)));

	// signal(SIGINT, stopGame);
	addFunctionStart(&terminalInit);
	addFunctionStart(&start);

	addFunctionLoop(&loop);

	addFunctionStop(&stop);

	setFps(200);
	startGame();

	return 0;
}
/*
	  > - > - > - > - > - > - > - > - > - > - > - > - > - > - > - > - > - > - > - > -
A			  v
A			  v
A			  v
< < < < < < < <

*/
