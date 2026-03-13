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

Vector2Int screenSafezone = {0, 10};
Vector2Int screenSize = {44, 22};
int borderSize = 1;
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
	printf("delta time: %10dns FPS: %3f\n nrOfChunks: %d\n", deltaTime, (1.0 / deltaTime * 1000000000), nrOfChunks(world));
	printf("Frame: %d  ", frame++);
	printf("Pos x: %-3d Pos y: %-3d", pos_x, pos_y);
	printf("pressed: %-10s released: %-10s held: %-10s \n", keyToString(keyEvent.pressed), keyToString(keyEvent.released), keyToString(keyEvent.held));
}
void redrawCanvasAndGui()
{
	printf("\033[2J\033[H");
	canvas = canvasNew(screenSize);
	NineRect nineRect = {
		{{(Sprite){'\\'}, (Sprite){'|'}, (Sprite){'/'}},
		 {(Sprite){'='}, (Sprite){'.'}, (Sprite){'='}},
		 {(Sprite){'/'}, (Sprite){'|'}, (Sprite){'\\'}}}};
	for (int i = 0; i < borderSize; i++)
	{
		canvasDrawNineRect(canvas, (Vector2Int){i, i}, (Vector2Int){screenSize.x - i * 2, screenSize.y - i * 2}, nineRect, FILL_NONE);
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
	Vector2Int termsize = getTermSize();
	termsize = vecSubI(termsize, screenSafezone);
	if (termsize.x != screenSize.x || termsize.y != screenSize.y)
	{
		screenSize = termsize;
		redrawCanvasAndGui();
	}

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
	Vector2Int actualScreenSize = vecSubI(screenSize, screenSafezone);
	Vector2Int position = {pos_x - screenSize.x / 2, pos_y - screenSize.y / 2};

	// writeAreaToCanvas(world, canvas, position, (Vector2Int)vectorAdd(position, actualScreenSize), (Vector2Int){0, 0});
	writeAreaToCanvas(world, canvas, position, (Vector2Int){screenSize.x - borderSize * 2, screenSize.y - borderSize * 2}, (Vector2Int){borderSize, borderSize});

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
	if ((frame/4) & 2)
		canvasSetSprite(canvas,
						vecDivI(screenSize, (Vector2Int){2, 2}),
						(Sprite){'U', COLOR_WHITE, COLOR_TRANSPARENT});

	writeCanvasToBuffer(canvas);
	printf("\033[H"); // scroll back terminal
	flush();
}

void start()
{
	printf("\033[2J");	// clear terminal
	printf("\33[?25l"); // reset ansi

	world = createWorld();
	redrawCanvasAndGui();
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
	addFunctionStop(&stop);
	addFunctionLoop(&loop);
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
