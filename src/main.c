#include "./utils/systemIndependant.h"
#include "./utils/perlin.h"
#include "./utils/map.h"
#include "./utils/vector2.h"
#include "./terminal/terminal.h"

#include "./graphical/renderer.h"
#include "./graphical/window.h"
#include "./graphical/canvas.h"

#include "./game/player.h"
#include "./game/gameLoop.h"

#include "./world/world.h"

#include "./gameData.h"
#include "./world/chunkGenerator.h"
#include "./world/pathFinder.h"
#include "./sound/sound.h"

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
Direction placeDirection = DIR_NORTH;
TileKind selectedTile = TILE_BELT;

void screenUpdate(Window *window);
void screenRender(Window *window, Canvas *canvas);
void gameUpdate(Window *window);
void gameRender(Window *window, Canvas *canvas);
void debugInfoUpdate(Window *window);
void debugInfoRender(Window *window, Canvas *canvas);
void ticker(double deltaTime);
Canvas *screenCanvas;
Window *gameWindow;

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
void render(double delta)
{
	gameData.frameDelta = delta;
	gameData.frame++;
	windowManagerRender(screenCanvas);
	rendererDrawCanvas(screenCanvas);
	terminalSetCursorVisible(false);
	rendererFlush();
}

void ticker(double deltaTime)
{
	gameData.tickDelta = deltaTime;
	poolInput();

	Vector2Int cursorPos = vecDivI(terminalGetMousePos(), (Vector2Int){1, 1});
	if (terminalGetKeyState(KEY_ESC) == KEY_JUST_PRESSED)
	{
		gameLoopStopGame();
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

	gameData.tick++;
	windowManagerUpdate(screenCanvas);
	windowManagerMouse(cursorPos);

	if (testEntity->path.result != PATHFINDER_ERROR && testEntity->path.length > 0 && testEntity->path.progress < testEntity->path.length)
	{
		Vector2Int newPos = testEntity->path.points[testEntity->path.progress];
		if (isTileWalkable(gameData.activeWorld, newPos))
		{

			soundPlay(&walkSound, 1, NULL);
			testEntity->position = newPos;
			testEntity->path.progress++;
		}
		else
		{
			if (testEntity->path.points != NULL)
				free(testEntity->path.points);
		}
	}

	worldTick(&gameData);
	// debugInfo(deltaTime, keyEvent);

	// render(deltaTime, &gameData);

	int chunkGenerateRadius = 3;
	for (int x = -chunkGenerateRadius; x < chunkGenerateRadius; x++)
	{
		for (int y = -chunkGenerateRadius; y < chunkGenerateRadius; y++)
			generateChunk(gameData.activeWorld,
						  (player->position.x + gameWindow->size.x / 2) + x * CHUNK_SIZE,
						  (player->position.y + gameWindow->size.y / 2) + y * CHUNK_SIZE,
						  generateCraterChunk);
	}
}

Vector2Int terminalToScreenSize(Canvas *canvas, Vector2Int terminalSize)
{
	return (Vector2Int){canvasGetDoubleSpaced(canvas) ? terminalSize.x / 2 : terminalSize.x, terminalSize.y};
}
void screenRender(Window *window, Canvas *canvas)
{
	// canvasFill(screenCanvas, (Sprite){.icon = ' '});
	printf("║");
	NineRect nineRect = {
		{{(Sprite){"╔", .colorFore = COLOR_WHITE}, (Sprite){"║", .colorFore = COLOR_WHITE}, (Sprite){"╚", .colorFore = COLOR_WHITE}},
		 {(Sprite){"═", .colorFore = COLOR_WHITE}, (Sprite){"║", .colorFore = COLOR_WHITE}, (Sprite){"═", .colorFore = COLOR_WHITE}},
		 {(Sprite){"╗", .colorFore = COLOR_WHITE}, (Sprite){"║", .colorFore = COLOR_WHITE}, (Sprite){"╝", .colorFore = COLOR_WHITE}}}};

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
	Vector2Int previewSize = getTileSize(selectedTile);
	Vector2Int previewOriginOffset = getTileOriginOffset(selectedTile);

	for (int x = 0; x < previewSize.x; x++)
	{
		for (int y = 0; y < previewSize.y; y++)
		{
			Vector2Int previewPos = vecAddI(buildPos, (Vector2Int){x, y});
			previewPos = vecSubI(previewPos, previewOriginOffset);

			TileDefinition *def = getTileDefinition(selectedTile);
			Sprite sprite = (Sprite){
				.icon.data = def->icon,
				.colorFore = COLOR_WHITE,
				.colorBack = COLOR_BLACK};
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
	if (testEntity->path.result != PATHFINDER_ERROR && testEntity->path.length > 0 && testEntity->path.progress < testEntity->path.length)
	{
		for (size_t i = testEntity->path.progress; i < testEntity->path.length; i++)
		{
			Vector2Int point = testEntity->path.points[i];
			Sprite sprite = (Sprite){
				.icon.data = '.',
				.colorFore = COLOR_WHITE,
				.colorBack = COLOR_BLACK};
			canvasSetSprite(canvas, worldToScreen(point), sprite);
			//  printf("{%d, %d}", point.x, point.y);
		}
	}
	Sprite sprite = (Sprite){
		.icon = "║",
		.colorFore = COLOR_WHITE,
		.colorBack = COLOR_BLACK};
	canvasSetSprite(canvas, worldToScreen(testEntity->position), sprite);
	// canvasSetSprite(canvas, worldToScreen(buildPos), sprite);

	terminalSetCursorPos((Vector2Int){0, 0});
	// canvasFill(canvas, sprite);
}
void gameUpdate(Window *window)
{
	window->position = (Vector2Int){1, 1};
	Vector2Int size = {
		.x = window->parent->size.x,
		.y = window->parent->size.y,
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
	window->position.x = window->parent->size.x - window->size.x - 2;
}
void debugInfoRender(Window *window, Canvas *canvas)
{
	// canvasFill(canvas, (Sprite){.icon = '-'});
	//  char text[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum";
	//   char text[] = "01234567891";
	Vector2Int pos = {0, 0};
	struct TextFormat format = {
		.textBoxSize = window->size,
		.foreground = (Color){255, 255, 255},
		.background = (Color){10, 10, 10},
		.trimStartWhitespace = true,
		.breakOnWords = true,
		.alignment = TEXT_ALIGN_RIGHT};

	// printf("\033[2K"); // clear line
	char buffer[48];

	snprintf(buffer, sizeof(buffer), "selected item[%d][%c]: %s", selectedTile, getTileDefinition(selectedTile)->icon, getTileDefinition(selectedTile)->name);
	pos.y += canvasWriteString(canvas, buffer, pos, &format).y;
	snprintf(buffer, sizeof(buffer), "screen width: %d height:%d", window->size.x, window->size.y);
	pos.y += canvasWriteString(canvas, buffer, pos, &format).y;
	snprintf(buffer, sizeof(buffer), "nrOfChunks: %d", nrOfChunks(gameData.activeWorld));
	pos.y += canvasWriteString(canvas, buffer, pos, &format).y;
	snprintf(buffer, sizeof(buffer), "Frame: %5d FPS: %.2f", gameData.frame++, 1 / gameData.frameDelta);
	pos.y += canvasWriteString(canvas, buffer, pos, &format).y;
	snprintf(buffer, sizeof(buffer), "Tick: %5d TPS: %.2f", gameData.tick++, 1 / gameData.tickDelta);
	pos.y += canvasWriteString(canvas, buffer, pos, &format).y;
	snprintf(buffer, sizeof(buffer), "Pos x: %-3d\nPos y: %-3d", (int)player->position.x, (int)player->position.y);
	pos.y += canvasWriteString(canvas, buffer, pos, &format).y;
	snprintf(buffer, sizeof(buffer), "placeDir %d", placeDirection);
	pos.y += canvasWriteString(canvas, buffer, pos, &format).y;

	/*
format.alignment = TEXT_ALIGN_LEFT;
canvasWriteString(canvas, text, vecSubI(terminalGetMousePos(), (Vector2Int){10, 10}), &format);

canvasWriteString(canvas, text, (Vector2Int){56, 56}, &format);
*/
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
		.size = {32, 20},
		.z = 4};
	windowManagerAddWindow(debugInfoDef);
	screenCanvas = canvasNew(screenWindow->size);

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
	exit(0);
}

int main(int argc, char const *argv[])
{
	signal(SIGINT, gameLoopStopGame);
	soundInit();
	terminalInit();

	gameLoopAddFunctionStart(&start);

	gameLoopAddFunctionLoop(&ticker, 30);
	gameLoopAddFunctionLoop(&render, 60);

	gameLoopAddFunctionStop(&stop);

	gameLoopStartGame();
	return 0;
}
