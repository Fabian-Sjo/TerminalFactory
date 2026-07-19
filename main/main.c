#include "../src/utils/systemIndependant.h"
#include "../src/utils/perlin.h"
#include "../src/utils/map.h"
#include "../src/utils/vector2.h"
#include "../src/terminal/terminal.h"

#include "../src/graphical/renderer.h"
#include "../src/graphical/window.h"
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

int borderSize = 1;

typedef struct Entity
{
	Vector2Int position;
	PathFinderPath path;
} Entity;
Sound walkSound;
GameData gameData;
Player *player;
Entity *testEntity;
Vector2Int buildPos;
Vector2Int gameScreenLocalMouse;
Vector2Int screenSize;
Direction placeDirection = DIR_NORTH;
TileKind selectedTile = TILE_BELT;

void screenUpdate(Window *window);
void screenRender(Window *window, Canvas *canvas);
void gameUpdate(Window *window);
void gameRender(Window *window, Canvas *canvas);
void debugInfoUpdate(Window *window);
void debugInfoRender(Window *window, Canvas *canvas);

Canvas *screenCanvas;
Window *gameWindow;

void render(double deltaTime, GameData *gameData);

Vector2Int screenToWorld(Vector2Int screenPos)
{

	return (Vector2Int){
		screenPos.x + floor(player->position.x),
		screenPos.y + floor(player->position.y)};
}
Vector2Int worldToScreen(Vector2Int worldPos)
{
	return (Vector2Int){
		worldPos.x - floor(player->position.x),
		worldPos.y - floor(player->position.y)};
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
void testPath()
{

	// printf("length: %d result: %d\n",testEntity->path.length,testEntity->path.result);
	//  printf("pos x: %f y: %f\n",
	//	   testEntity->position.x,
	//	   testEntity->position.y);
	if (testEntity->path.result != PATHFINDER_ERROR && testEntity->path.length > 0)
	{
		for (size_t i = 0; i < testEntity->path.length; i++)
		{
			Vector2Int point = testEntity->path.points[i];
			Sprite sprite = (Sprite){.icon = '.', COLOR_WHITE, COLOR_BLACK};
			// canvasSetSprite(gameCanvas, worldToScreen(point), sprite);
			//  printf("{%d, %d}", point.x, point.y);
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
	poolInput();

	Vector2Int cursorPos = vecDivI(terminalGetMousePos(), (Vector2Int){1, 1});
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
		if (gameWindow->scale.x == 1)
			gameWindow->scale.x = 2;
		else
			gameWindow->scale.x = 1;
	};
	if (terminalGetKeyState(KEY_V) == KEY_JUST_PRESSED)
	{

		if (testEntity->path.points != NULL)
			free(testEntity->path.points);
		testEntity->path = getPath(
			testEntity->position,
			screenToWorld(windowManagerLocalizeMousePos(cursorPos, *gameWindow)),
			gameData.activeWorld);
	};
	if (terminalGetKeyState(KEY_SPACE))
	{
		removeTile(gameData.activeWorld, screenToWorld(buildPos));
	};
	if (terminalGetKeyState(KEY_MOUSE_1))
	{
		placeTile(gameData.activeWorld, buildPos, placeDirection, selectedTile);
	};

	gameData.frame++;
	gameData.tick++;
	windowManagerUpdate(screenCanvas);
	windowManagerMouse(cursorPos);

	windowManagerRender(screenCanvas);

	worldTick(&gameData);
	// debugInfo(deltaTime, keyEvent);

	// render(deltaTime, &gameData);
	rendererDrawCanvas(screenCanvas);
	terminalSetCursorVisible(false);
	rendererFlush();
	/*
	printf("selected item[%d][%c]: %s        \n", selectedTile, getTileDefinition(selectedTile)->icon, getTileDefinition(selectedTile)->name);
	printf("screen width: %d height:%d\n", screenSize.x, screenSize.y);
	printf("delta time: %10fs FPS: %3f\n nrOfChunks: %d\n", deltaTime, (1.0 / deltaTime), nrOfChunks(gameData.activeWorld));
	printf("Frame: %d  ", gameData.frame++);
	printf("Pos x: %-3f Pos y: %-3f\n", player->position.x, player->position.y);
	printf("gameLocalMouse: Pos x: %-3d Pos y: %-3d\nbuildPos:       Pos x: %-3d Pos y: %-3d\n",
		   gameScreenLocalMouse.x,
		   gameScreenLocalMouse.y,
		   buildPos.x,
		   buildPos.y);
	printf("placeDir %d\n", placeDirection);
	*/
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
void screenRender(Window *window, Canvas *canvas)
{
	// canvasFill(screenCanvas, (Sprite){.icon = ' '});
	NineRect nineRect = {
		{{(Sprite){'\\'}, (Sprite){'|'}, (Sprite){'/'}},
		 {(Sprite){'='}, (Sprite){'.'}, (Sprite){'='}},
		 {(Sprite){'/'}, (Sprite){'|'}, (Sprite){'\\'}}}};

	canvasDrawNineRect(canvas,
					   vecSubI(window->position, (Vector2Int){0, 0}),
					   vecAddI(window->size, (Vector2Int){0, 0}),
					   nineRect, FILL_NONE);
}
void screenUpdate(Window *window)
{
	Vector2Int termsize = vecDivI(getTermSize(), (Vector2Int){1 + canvasGetDoubleSpaced(screenCanvas), 1});
	termsize.y -= 2;

	canvasSetSize(screenCanvas, termsize);
	canvasFill(screenCanvas, (Sprite){.icon = ' '});
	window->size = termsize;
}
void gameRender(Window *window, Canvas *canvas)
{

	canvasFill(canvas, (Sprite){.icon = ' '});

	Vector2Int position = {
		floor(player->position.x),
		floor(player->position.y)};
	//{
	//	floor(player->position.x) - screenSize.x / 2,
	//	floor(player->position.y) - screenSize.y / 2};
	Vector2Int canvasSize = (Vector2Int){
		window->size.x - borderSize * 2,
		window->size.y - borderSize * 2};
	Vector2Int borders = (Vector2Int){borderSize, borderSize};
	writeAreaToCanvas(gameData.activeWorld, canvas, vecAddI(position, borders), canvasSize, borders, &gameData);
	testPath();
	Vector2Int previewSize = getTileSize(selectedTile);
	Vector2Int previewOriginOffset = getTileOriginOffset(selectedTile);

	for (int x = 0; x < previewSize.x; x++)
	{
		for (int y = 0; y < previewSize.y; y++)
		{
			Vector2Int previewPos = vecAddI(buildPos, (Vector2Int){x, y});
			previewPos = vecSubI(previewPos, previewOriginOffset);

			TileDefinition *def = getTileDefinition(selectedTile);
			Sprite sprite = (Sprite){def->icon, COLOR_WHITE, COLOR_BLACK};
			if (def->getSprite != NULL)
			{
				sprite = def->getSprite(placeDirection, (Vector2Int){x, y}, &gameData);
			}
			if (!canPlaceTile(gameData.activeWorld, vecAddI(buildPos, vecSubI((Vector2Int){x, y}, previewOriginOffset)), selectedTile))
			{
				sprite.colorFore = (Color){100, 0, 0};
			}
			else
			{
				sprite.colorFore = (Color){0, 30, 0};
			}
			sprite.colorBack = COLOR_TRANSPARENT;
			canvasSetSprite(canvas, worldToScreen(previewPos), sprite);
		}
	}
	Sprite sprite = (Sprite){.icon = '@', (Color){100, 100, 0}, COLOR_BLACK};
	canvasSetSprite(canvas, worldToScreen(testEntity->position), sprite);
	// canvasSetSprite(canvas, worldToScreen(buildPos), sprite);

	terminalSetCursorPos((Vector2Int){0, 0});
	// canvasFill(canvas, sprite);
}
void gameUpdate(Window *window)
{
	return;
	Vector2Int size = {
		.x = window->parent->size.x / 2,
		.y = window->parent->size.y / 2,
	};
	window->size = size;
}
bool gameMouse(Window *window, Vector2Int localMousePos, Vector2Int globalMousePos)
{
	/*terminalSetCursorPos(
		vecMulI(
			vecDivI(
				globalMousePos,
				canvasGetDisplayScale(gameCanvas)),
			canvasGetDisplayScale(gameCanvas)));
			terminalSetCursorVisible(true);
*/
	gameScreenLocalMouse = localMousePos;
	buildPos = screenToWorld(localMousePos);

	return true;
};
void debugInfoUpdate(Window *window)
{
}
void debugInfoRender(Window *window, Canvas *canvas)
{
	canvasFill(canvas, (Sprite){.icon = 'X'});

	char text[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum";
	struct TextFormat format = {
		.textBoxSize = (Vector2Int){40, 30},
		.foreground = (Color){0},
		.background = (Color){0},
		.trimStartWhitespace = true,
		.breakOnWords = true};

	format.alignment = TEXT_ALIGN_MID;
	canvasWriteString(canvas, text, (Vector2Int){33, 33}, &format);
	format.alignment = TEXT_ALIGN_LEFT;
	canvasWriteString(canvas, text, vecSubI(terminalGetMousePos(), (Vector2Int){10, 10}), &format);

	canvasWriteString(canvas, text, (Vector2Int){56, 56}, &format);
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

	Window screenWindowDef = {
		.position = {0, 0},
		.parent = NULL,
		.anchor = NULL,
		.scale = {1, 1},
		.render = &screenRender,
		.update = &screenUpdate,
		.size = {20, 20},
		.z = 2};
	Window *screenWindow = windowManagerAddWindow(screenWindowDef);
	Window gameWindowDef = {
		.position = {3, 3},
		.parent = screenWindow,
		.anchor = NULL,
		.scale = {1, 1},
		.render = &gameRender,
		.update = &gameUpdate,
		.mouse = &gameMouse,
		.size = {50, 100},
		.z = 2};
	gameWindow = windowManagerAddWindow(gameWindowDef);
	Window debugInfoDef = {
		.position = {3, 3},
		.parent = screenWindow,
		.anchor = NULL,
		.scale = {1, 1},
		.update = &debugInfoUpdate,
		.render = &debugInfoRender,
		.size = {10, 20},
		.z = 4};
	windowManagerAddWindow(debugInfoDef);
	screenCanvas = canvasNew(screenWindow->size);
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
