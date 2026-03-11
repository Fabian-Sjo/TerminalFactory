#include "utils/systemIndependant.h"
#include "utils/perlin.h"
#include "graphical/renderer.h"
#include "game/input.h"
#include "world/world.h"
#include "utils/map.h"

#include <stdio.h>
#include <signal.h>

#include "game/gameLoop.h"
#define SCREEN_WIDTH 100
#define SCREEN_HEIGHT 40

int TERRAIN_PARTS[] = {SPRITE_WATER_DEEP, SPRITE_WATER, SPRITE_SAND, SPRITE_GRASS, SPRITE_STONE, SPRITE_SNOW};
int pos_x = -20;
int pos_y = -20;
float zoom = 0.1;
char board[SCREEN_WIDTH][SCREEN_HEIGHT];
int frame = 0;

World *world = NULL;

void render();

void debugInfo(long deltaTime, KeyEvent keyEvent) {
	printf("delta time: %10dns\n", deltaTime);
	printf("Frame: %d  ", frame++);
	printf("Pos x: %-3d Pos y: %-3d", pos_x, pos_y);
	printf("pressed: %-10s released: %-10s held: %-10s ", keyToString(keyEvent.pressed), keyToString(keyEvent.released), keyToString(keyEvent.held));
}

void loop(long deltaTime)
{
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

	for (int x = 0; x < SCREEN_WIDTH; x++)
	{
		for (int y = 0; y < SCREEN_HEIGHT; y++)
		{
			float perlinValue = perlin_Get2d(
				(x - SCREEN_WIDTH / 2) * zoom + pos_x,
				(y - SCREEN_HEIGHT / 2) * zoom + pos_y,
				0.1, 1);
			if (perlinValue >= 1)
				perlinValue = 0.999999;
			int terrainIndex = perlinValue * (sizeof(TERRAIN_PARTS) / sizeof(TERRAIN_PARTS[0]));
			board[x][y] = TERRAIN_PARTS[terrainIndex];
			// board[x][y] = (x + pos_x) % 10 + ((y + pos_y) % 10) * 10;
		}
	}
	// printf("\033[2J"); // Clear entire screen
	// printf("\033[H");  // Move cursor to home position

	// pos_x++;

	generateChunk(world, pos_x + SCREEN_WIDTH / 2, pos_y + SCREEN_HEIGHT / 2);

	render();
}
void render()
{
	printf("\033[H"); // scroll back terminal

	for (int y = 0; y < SCREEN_HEIGHT; y++)
	{
		for (int x = 0; x < SCREEN_WIDTH; x++)
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

			//addCharToBuffer(' ');
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
