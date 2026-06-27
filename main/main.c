#include "../src/utils/systemIndependant.h"
#include "../src/utils/perlin.h"
#include "../src/utils/map.h"
#include "../src/utils/vector2.h"
#include "../src/terminal/terminal.h"

#include "../src/graphical/renderer.h"
#include "../src/graphical/canvas.h"

#include "../src/game/player.h"
#include "../src/game/gameLoop.h"

#include "../src/world/world.h"

#include "../src/gameData.h"
#include "../src/world/chunkGenerator.h"
#include "../src/world/pathFinder.h"
#include "../src/sound/sound.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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
typedef struct Entity
{
	Vector2Int position;
	PathFinderPath path;
} Entity;
Sound walkSound;
GameData gameData;
Player *player;
Entity *testEntity;
Vector2Int cursorPos;
Direction placeDirection = DIR_NORTH;
TileKind selectedTile = TILE_BELT;

void render(double deltaTime, GameData *gameData);
void tickPlayer(double deltaTime, GameData gameData);
Vector2Int screenToWorld(Vector2Int screenPos)
{

	return (Vector2Int){
		screenPos.x + floor(player->position.x) - gameData.screenSize.x / 2,
		screenPos.y + floor(player->position.y) - gameData.screenSize.y / 2};
}
Vector2Int worldToScreen(Vector2Int worldPos)
{
	return (Vector2Int){
		worldPos.x - floor(player->position.x) + gameData.screenSize.x / 2,
		worldPos.y - floor(player->position.y) + gameData.screenSize.y / 2};
}
void debugInfo(double deltaTime, GameData *gameData)
{

	// printf("\033[2K"); // clear line
	printf("selected item[%d][%c]: %s        \n", selectedTile, getTileDefinition(selectedTile)->icon, getTileDefinition(selectedTile)->name);
	printf("screen width: %d height:%d\n", gameData->screenSize.x, gameData->screenSize.y);
	printf("delta time: %10fs FPS: %3f\n nrOfChunks: %d\n", deltaTime, (1.0 / deltaTime), nrOfChunks(gameData->activeWorld));
	printf("Frame: %d  ", gameData->frame++);
	printf("Pos x: %-3f Pos y: %-3f\n", player->position.x, player->position.y);
	printf("mouse: Pos x: %-3d Pos y: %-3d mouseWorld: Pos x: %-3d Pos y: %-3d\n", cursorPos.x, cursorPos.y, screenToWorld(cursorPos).x, screenToWorld(cursorPos).y);
	printf("placeDir %d\n", placeDirection);

	for (int i = -30; i < 30; i++)
	{
		Vector2Int p = {.x = i, .y = i};
		Vector2Int toWorld = screenToWorld(p);
		Vector2Int pp = worldToScreen(toWorld);
		// assert(p.x == pp.x);
		// assert(p.y == pp.y);
	}
	// KeyEvent keyEvent = gameData->keyevent;
	// printf("pressed: %-10s released: %-10s held: %-10s \n", keyToString(keyEvent.pressed), keyToString(keyEvent.released), keyToString(keyEvent.held));
}

void redrawCanvasAndGui(GameData *gameData)
{
	printf("\033[2J\033[H");
	canvasSetSize(gameData->canvas, gameData->screenSize);
	// gameData->canvas = canvasNew(gameData->screenSize);
	// canvasSetDoubleSpaced(gameData->canvas, true);
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

SoundGeneratorResult walkSoundGenerator(double time)
{

	float freq =
		400.0f - time * 800.0f;

	float env =
		exp(-10 * time);

	return (SoundGeneratorResult){
		.val = sin(1 * 3.14 * freq * time) * env, .isFinished = time > 0.2};
}
void testPath(double deltaTime)
{

	printf("length: %d result: %d\n",
		   testEntity->path.length,
		   testEntity->path.result);
	// printf("pos x: %f y: %f\n",
	//	   testEntity->position.x,
	//	   testEntity->position.y);
	if (testEntity->path.result != PATHFINDER_ERROR && testEntity->path.length > 0)
	{
		for (size_t i = 0; i < testEntity->path.length; i++)
		{
			Vector2Int point = testEntity->path.points[i];
			Sprite sprite = (Sprite){.icon = '.', COLOR_WHITE, COLOR_BLACK};
			canvasSetSprite(gameData.canvas, worldToScreen(point), sprite);
			// printf("{%d, %d}", point.x, point.y);
		}

		Vector2Int newPos = testEntity->path.points[1];

		if (isTileWalkable(gameData.activeWorld, newPos))
		{

			soundPlay(&walkSound, 1, NULL);
			testEntity->position = newPos;
		}
		Vector2Int oldTarget = testEntity->path.points[testEntity->path.length - 1];
		if (testEntity->path.points != NULL)
			free(testEntity->path.points);
		testEntity->path = getPath(testEntity->position, oldTarget, gameData.activeWorld);
	}
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
			generateChunk(gameData.activeWorld, player->position.x + x * 8, player->position.y + y * 8, &generateMoonChunk);
	}
}
Vector2Int terminalToScreenSize(Canvas *canvas, Vector2Int terminalSize)
{
	return (Vector2Int){canvasGetDoubleSpaced(canvas) ? terminalSize.x / 2 : terminalSize.x, terminalSize.y};
}
void render(double deltaTime, GameData *gameData)
{
	Vector2Int termsize = vecDivI(vecSubI(getTermSize(), screenSafezone), (Vector2Int){1, 1});

	Vector2Int newScreenSize = terminalToScreenSize(gameData->canvas, termsize);

	if (newScreenSize.x != gameData->screenSize.x || newScreenSize.y != gameData->screenSize.y)
	{
		gameData->screenSize = newScreenSize;
		redrawCanvasAndGui(gameData);
	}

	Vector2Int position = {
		floor(player->position.x) - floor(gameData->screenSize.x / 2),
		floor(player->position.y) - floor(gameData->screenSize.y / 2)};
	//{
	//	floor(player->position.x) - gameData->screenSize.x / 2,
	//	floor(player->position.y) - gameData->screenSize.y / 2};
	Vector2Int canvasSize = (Vector2Int){
		gameData->screenSize.x - borderSize * 2,
		gameData->screenSize.y - borderSize * 2};
	Vector2Int borders = (Vector2Int){borderSize, borderSize};
	writeAreaToCanvas(gameData->activeWorld, gameData->canvas, vecAddI(position, borders), canvasSize, borders, gameData);
	testPath(deltaTime);
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
	Sprite sprite = (Sprite){.icon = '@', (Color){100, 100, 0}, COLOR_BLACK};
	canvasSetSprite(gameData->canvas, worldToScreen(testEntity->position), sprite);

	terminalSetCursorPos((Vector2Int){0, 0});
	rendererDrawCanvas(gameData->canvas);
	rendererFlush();
	// terminalDrawCanvas(gameData->canvas);
	// terminalDraw();
}
Vector2Int canvasMousePos(Canvas *canvas)
{
}
void tickPlayer(double deltaTime, GameData gameData)
{

	poolInput();
	cursorPos = vecDivI(terminalGetMousePos(), (Vector2Int){1 + canvasGetDoubleSpaced(gameData.canvas), 1});
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
	if (terminalGetKeyState(KEY_S))
	{
		player->position.y++;
		// player->position.y += deltaTime * player->speed;
	};
	if (terminalGetKeyState(KEY_W))
	{
		player->position.y--;
		// player->position.y -= deltaTime * player->speed;
	};
	if (terminalGetKeyState(KEY_A))
	{
		player->position.x--;
		// player->position.x -= deltaTime * player->speed;
	};
	if (terminalGetKeyState(KEY_D))
	{
		player->position.x++;
		// player->position.x += deltaTime * player->speed;
	};
	if (terminalGetKeyState(KEY_C) == KEY_JUST_PRESSED)
	{
		canvasSetDoubleSpaced(gameData.canvas, !canvasGetDoubleSpaced(gameData.canvas));
	};
	if (terminalGetKeyState(KEY_V) == KEY_JUST_PRESSED)
	{

		if (testEntity->path.points != NULL)
			free(testEntity->path.points);
		testEntity->path = getPath(testEntity->position, screenToWorld(cursorPos), gameData.activeWorld);
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
	walkSound = (Sound){
		.type = SOUND_TYPE_GENERATOR,
		.source.generator = {
			.generator = &walkSoundGenerator}};

	player = playerNew();
	testEntity = malloc(sizeof(Entity));
	testEntity->position = (Vector2Int){0, 0};
	testEntity->path.length = 0;
	testEntity->path.points = NULL;

	printf("\033[2J");	// clear terminal
	printf("\33[?25l"); // reset ansi
	World *world = createWorld();
	gameData.activeWorld = world;
	gameData.canvas = canvasNew((Vector2Int){30, 30});
	int chunkGenerateRadius = 3;
	for (int x = -chunkGenerateRadius; x < chunkGenerateRadius; x++)
	{
		for (int y = -chunkGenerateRadius; y < chunkGenerateRadius; y++)
			generateChunk(world, x * 8, y * 8, &generateMoonChunk);
	}

	FILE *fptr = fopen("sample-12s.wav", "rb");
	if (!fptr)
	{
		printf("failed to open file");
		return 0;
	}
	Sound *music = parseWav(fptr);

	// soundStart(music, 9, 0, true, NULL);

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

void gameInit()
{
	// has to be power of 2
	// assert(CHUNK_SIZE && !(CHUNK_SIZE & (CHUNK_SIZE - 1)));

	// signal(SIGINT, stopGame);
	soundInit();
	terminalInit();

	addFunctionStart(&start);

	addFunctionLoop(&loop);

	addFunctionStop(&stop);

	setFps(60);
	startGame();
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
	return 0;
}
