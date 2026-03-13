#include "utils/systemIndependant.h"
#include "utils/perlin.h"
#include "utils/map.h"

#include "graphical/renderer.h"
#include "graphical/canvas.h"

#include "game/player.h"
#include "game/input.h"
#include "game/gameLoop.h"

#include "world/world.h"
#include "world/chunk.h"

#include <assert.h>
#include <stdio.h>
#include <signal.h>

Vector2Int screenSafezone = {0, 10};
int borderSize = 1;

// World *world = NULL;
// Canvas *canvas = NULL;

typedef struct GameData
{
	KeyEvent keyevent;
	Player *player;

	int frame;

	Vector2Int screenSize;

	World *activeWorld;
	Canvas *canvas;
} GameData;
GameData gameData;

void render(GameData *gameData);
void tickPlayer(GameData *gameData);

void debugInfo(long deltaTime, GameData *gameData)
{

	printf("screen width: %d height:%d\n", gameData->screenSize.x, gameData->screenSize.y);
	printf("delta time: %10dms FPS: %3f\n nrOfChunks: %d\n", deltaTime, (1.0 / deltaTime * 1000), nrOfChunks(gameData->activeWorld));
	printf("Frame: %d  ", gameData->frame++);
	printf("Pos x: %-3d Pos y: %-3d", gameData->player->position.x, gameData->player->position.y);
	KeyEvent keyEvent = gameData->keyevent;
	printf("pressed: %-10s released: %-10s held: %-10s \n", keyToString(keyEvent.pressed), keyToString(keyEvent.released), keyToString(keyEvent.held));
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
void loop(long deltaTime)
{

	gameData.keyevent = getKeyEvent();
	gameData.frame++;
	tickPlayer(&gameData);
	// debugInfo(deltaTime, keyEvent);
	generateChunk(gameData.activeWorld, gameData.player->position.x, gameData.player->position.y);

	render(&gameData);
}
void render(GameData *gameData)
{
	Vector2Int termsize = getTermSize();
	termsize = vecSubI(termsize, screenSafezone);
	if (termsize.x != gameData->screenSize.x || termsize.y != gameData->screenSize.y)
	{
		gameData->screenSize = termsize;
		redrawCanvasAndGui(gameData);
	}
	Vector2Int actualScreenSize = vecSubI(gameData->screenSize, screenSafezone);
	Vector2Int position = {
		gameData->player->position.x - gameData->screenSize.x / 2,
		gameData->player->position.y - gameData->screenSize.y / 2};

	writeAreaToCanvas(gameData->activeWorld, gameData->canvas, position,
					  (Vector2Int){
						  gameData->screenSize.x - borderSize * 2,
						  gameData->screenSize.y - borderSize * 2},
					  (Vector2Int){borderSize, borderSize});

	// cursor
	if ((gameData->frame / 4) & 2)
		canvasSetSprite(gameData->canvas,
						vecAddI(vecDivI(gameData->screenSize, (Vector2Int){2, 2}),(Vector2Int){1,1}),
						(Sprite){'_', COLOR_WHITE, COLOR_TRANSPARENT});

	writeCanvasToBuffer(gameData->canvas);
	printf("\033[H"); // scroll back terminal
	flush();
}
void tickPlayer(GameData *gameData)
{

	switch (gameData->keyevent.pressed)
	{
	case KEY_ESC:
		stopGame();
		break;
	case KEY_S:
		gameData->player->position.y++;
		break;
	case KEY_W:
		gameData->player->position.y--;
		break;
	case KEY_A:
		gameData->player->position.x--;
		break;
	case KEY_D:
		gameData->player->position.x++;
		break;
	case KEY_SPACE:
		setTile(gameData->activeWorld, gameData->player->position, &testTile);
		break;
	default:
		break;
	}
	switch (gameData->keyevent.held)
	{
	case KEY_S:
		gameData->player->position.y++;
		break;
	case KEY_W:
		gameData->player->position.y--;
		break;
	case KEY_A:
		gameData->player->position.x--;
		break;
	case KEY_D:
		gameData->player->position.x++;
		break;

	default:
		break;
	}
}
void start()
{
	printf("\033[2J");	// clear terminal
	printf("\33[?25l"); // reset ansi

	World *world = createWorld();
	gameData.activeWorld = world;
	gameData.canvas = canvasNew((Vector2Int){30, 30});
	gameData.player = playerNew();

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
	assert(CHUNK_SIZE && !(CHUNK_SIZE & (CHUNK_SIZE - 1)));

	signal(SIGINT, stopGame);

	printf("\033[0");
	addFunctionStart(&initInput);
	addFunctionStart(&start);

	addFunctionLoop(&loop);

	addFunctionStop(&stop);

	setFps(30);
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
