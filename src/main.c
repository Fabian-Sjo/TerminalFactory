#include "utils/systemIndependant.h"
#include "utils/perlin.h"
#include "utils/map.h"

#include "graphical/renderer.h"
#include "graphical/canvas.h"

#include "game/input.h"
#include "game/gameLoop.h"

#include "world/world.h"
#include "world/chunk.h"

#include <assert.h>
#include <stdio.h>
#include <signal.h>

Vector2Int screenSafezone = {8, 10};
Vector2Int screenSize = {44, 22};

int TERRAIN_PARTS[] = {SPRITE_WATER_DEEP, SPRITE_WATER, SPRITE_SAND, SPRITE_GRASS, SPRITE_STONE, SPRITE_SNOW};
int pos_x = 20;
int pos_y = 20;
float zoom = 0.1f;

int frame = 0;

World *world = NULL;
Canvas *canvas = NULL;
void render();

void debugInfo(long deltaTime, KeyEvent keyEvent)
{
	printf("screen width: %d height:%d\n", screenSize.x, screenSize.y);
	printf("delta time: %10dns FPS: %3f\n", deltaTime, (1.0 / deltaTime * 1000000000));
	printf("Frame: %d  ", frame++);
	printf("Pos x: %-3d Pos y: %-3d", pos_x, pos_y);
	printf("pressed: %-10s released: %-10s held: %-10s \n", keyToString(keyEvent.pressed), keyToString(keyEvent.released), keyToString(keyEvent.held));
}

void loop(long deltaTime)
{
	screenSize = getTermSize();
	KeyEvent keyEvent = getKeyEvent();
	debugInfo(deltaTime, keyEvent);
	switch (keyEvent.pressed)
	{
	case KEY_Q:
		zoom += 0.1;
		break;
	case KEY_E:
		zoom -= 0.1;
		break;
	case KEY_ESC:
		stopGame();
		break;
	case KEY_S:
		pos_y++;
		break;
	case KEY_W:
		pos_y--;
		break;
	case KEY_A:
		pos_x--;
		break;
	case KEY_D:
		pos_x++;
		break;
	default:
		break;
	}
	switch (keyEvent.held)
	{
	case KEY_S:
		pos_y++;
		break;
	case KEY_W:
		pos_y--;
		break;
	case KEY_A:
		pos_x--;
		break;
	case KEY_D:
		pos_x++;
		break;

	default:
		break;
	}

	// printf("\033[2J"); // Clear entire screen
	// printf("\033[H");  // Move cursor to home position

	// pos_x++;

	generateChunk(world, pos_x, pos_y);

	render();
}
void render()
{
	Vector2Int actualScreenSize = vectorSub(screenSize, screenSafezone);
	Vector2Int position = {pos_x, pos_y};

	// writeAreaToCanvas(world, canvas, position, (Vector2Int)vectorAdd(position, actualScreenSize), (Vector2Int){0, 0});
	writeAreaToCanvas(world, canvas, position, (Vector2Int){10, 10}, (Vector2Int){10, 10});

	addCanvasToBuffer(canvas);
	// for (int y = 0; y < actualScreenSize.y; y++)
	//{
	//	for (int x = 0; x < actualScreenSize.x; x++)
	//	{
	//
	//		addSpriteToBuffer(sprites[y * actualScreenSize.x + x]);
	//		// addSpriteToBuffer((Sprite){'X', {200, 0, 0}, COLOR_BLACK});
	//		// addCharToBuffer('*');
	//	}
	//	newLine();
	//}

	// addCharToBuffer(((y + pos_y) / 10) % 10 + '0');
	// addCharToBuffer((y + pos_y) % 10 + '0');

	flush();
	printf("wewew");  // scroll back terminal
	printf("\033[H"); // scroll back terminal
}
void start()
{
	printf("\033[2J");	// clear terminal
	printf("\33[?25l"); // reset ansi

	world = createWorld();
	canvas = canvasNew(screenSize);
	cavasDrawRectangle(canvas, (Vector2Int){2, 2}, (Vector2Int){5, 5}, (Sprite){'#'}, FILL_ALL);
	cavasDrawRectangle(canvas, (Vector2Int){8, 2}, (Vector2Int){5, 5}, (Sprite){'+'}, FILL_NONE);
	NineRect nineRect = {
		{{(Sprite){'#'}, (Sprite){'v'}, (Sprite){'#'}},
		 {(Sprite){'<'}, (Sprite){'.'}, (Sprite){'>'}},
		 {(Sprite){'#'}, (Sprite){'^'}, (Sprite){'#'}}}};
	canvasDrawNineRect(canvas, (Vector2Int){14, 2}, (Vector2Int){5, 5}, nineRect, FILL_ALL);
	canvasDrawNineRect(canvas, (Vector2Int){20, 2}, (Vector2Int){10, 10}, nineRect, FILL_NONE);
	//▶ ◀ ▲ ▼
	NineRect nineRect2 = {
		{{(Sprite){'#'}, (Sprite){'▼'}, (Sprite){'#'}},
		 {(Sprite){'◀'}, (Sprite){'.'}, (Sprite){'▶'}},
		 {(Sprite){'#'}, (Sprite){'▲'}, (Sprite){'#'}}}};
	canvasDrawNineRect(canvas, (Vector2Int){32, 2}, (Vector2Int){10, 10}, nineRect2, FILL_NONE);
	// areaAsSprites(world, (Vector2Int){1, 1}, (Vector2Int){3, 3});

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

	// signal(SIGINT, stopGame);
	printf("\033[0");
	addFunctionStart(&initInput);
	addFunctionStart(&start);
	addFunctionStop(&stop);
	addFunctionLoop(&loop);
	setFps(30000);
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
