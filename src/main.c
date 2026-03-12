#include "utils/systemIndependant.h"
#include "utils/perlin.h"
#include "graphical/renderer.h"
#include "game/input.h"
#include "world/chunk.h"
#include "world/world.h"
#include "utils/map.h"

#include <stdio.h>
#include <signal.h>

#include "game/gameLoop.h"

#define RENDER_DISTANCE 12

Vector2Int screenSafezone = {5, 5};
Vector2Int screenSize = {10, 10};

int TERRAIN_PARTS[] = {SPRITE_WATER_DEEP, SPRITE_WATER, SPRITE_SAND, SPRITE_GRASS, SPRITE_STONE, SPRITE_SNOW};
int pos_x = -20;
int pos_y = -20;
float zoom = 0.1f;

int frame = 0;

World *world = NULL;

void render();

void debugInfo(long deltaTime, KeyEvent keyEvent)
{
	printf("screen width: %d height:%d\n", screenSize.x, screenSize.y);
	printf("delta time: %10dns\n", deltaTime);
	printf("Frame: %d  ", frame++);
	printf("Pos x: %-3d Pos y: %-3d", pos_x, pos_y);
	printf("pressed: %-10s released: %-10s held: %-10s ", keyToString(keyEvent.pressed), keyToString(keyEvent.released), keyToString(keyEvent.held));
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
		pos_x -= 2;
		break;
	case KEY_D:
		pos_x += 2;
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
		pos_x -= 2;
		break;
	case KEY_D:
		pos_x += 2;
		break;

	default:
		break;
	}

	// printf("\033[2J"); // Clear entire screen
	// printf("\033[H");  // Move cursor to home position

	// pos_x++;

	for (int x = pos_x - RENDER_DISTANCE*CHUNK_SIZE/2; x < pos_x + RENDER_DISTANCE*CHUNK_SIZE/2; x++) {
		for (int y = pos_y - RENDER_DISTANCE*CHUNK_SIZE/2; y < pos_y + RENDER_DISTANCE/2; y++) {
            	generateChunk(world, x, y);
            }
        }

	render();
}
void render()
{
	printf("\033[H"); // scroll back terminal
	Vector2Int actualScreenSize = vectorSub(screenSize, screenSafezone);
	for (int y = -actualScreenSize.y / 2; y < actualScreenSize.y / 2; y++)
	{
		for (int x = -actualScreenSize.x / 2; x < actualScreenSize.x / 2; x++)
		{

			GroundTile *groundTile = getGroundTile(world, x + pos_x, y + pos_y);
			Sprite groundSprite = {' ', {200, 0, 0}, COLOR_BLACK};

			groundSprite = getGroundTileSprite(groundTile);

			Tile *tile = getTile(world, x + pos_x, y + pos_y);
			if (tile != NULL)
			{
				getTile(world, x + pos_x, y + pos_y);
				Sprite sprite = getTileSprite(tile);

				groundSprite.icon = sprite.icon;
				groundSprite.colorFore = sprite.colorFore;
			}

			addSpriteToBuffer(groundSprite);

			// addCharToBuffer(' ');
		}
		addCharToBuffer(((y + pos_y) / 10) % 10 + '0');
		addCharToBuffer((y + pos_y) % 10 + '0');
		newLine();
	}
	flush();
}
void start()
{
	printf("\033[2J");	// clear terminal
	printf("\33[?25l"); // reset ansi

	world = createWorld();
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
	// signal(SIGINT, stopGame);

	printf("\033[0");
	addFunctionStart(&initInput);
	addFunctionStart(&start);
	addFunctionStop(&stop);
	addFunctionLoop(&loop);
	setFps(50);
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
