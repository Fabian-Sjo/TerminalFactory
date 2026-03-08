#include "systemIndependant.h"
#include "perlin.h"
#include "renderer.h"
#include "input.h"

#include "gameLoop.h"
#define BOARD_WIDTH 40
#define BOARD_HEIGHT 40

int TERRAIN_PARTS[] = {SPRITE_WATER_DEEP, SPRITE_WATER, SPRITE_SAND, SPRITE_GRASS};
int pos_x = 10;
int pos_y = 10;

char board[BOARD_WIDTH][BOARD_HEIGHT];
int frame = 0;

void loop(long deltaTime)
{
	printf("delta time: %10dns\n", deltaTime);
	KeyEvent keyEvent = getKeyEvent();
	printf("Frame %d  ", frame++);
	printf("pressed: %-10s released: %-10s held: %-10s ", keyToString(keyEvent.pressed), keyToString(keyEvent.released), keyToString(keyEvent.held));

	switch (keyEvent.held)
	{
	case KEY_DOWN_ARROW:
		pos_y++;
		break;
	case KEY_UP_ARROW:
		pos_y--;
		break;
	case KEY_LEFT_ARROW:
		pos_x--;
		break;
	case KEY_RIGHT_ARROW:
		pos_x++;
		break;

	default:
		break;
	}

	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			int terrainIndex = perlin_Get2d(x + pos_x, y + pos_y, 0.1, 1) * (sizeof(TERRAIN_PARTS) / sizeof(TERRAIN_PARTS[0]));
			board[x][y] = TERRAIN_PARTS[terrainIndex];
			// board[x][y] = (x + pos_x) % 10 + ((y + pos_y) % 10) * 10;
		}
	}
	// printf("\033[2J"); // Clear entire screen
	// printf("\033[H");  // Move cursor to home position
	printf("\033[H"); // scroll back terminal
	// printf("\e[2J"); // clear terminal
	//pos_x++;
	for (int y = 0; y < BOARD_HEIGHT; y++)
	{
		for (int x = 0; x < BOARD_WIDTH; x++)
		{

			char thisChar = board[x][y];

			// setColorFore(x+y);
			// setColorBack(x+y);
			// addCharToBuffer(y % 10 + '0');
			// addCharToBuffer((x / 10) % 10 + '0');
			// addCharToBuffer(thisChar);
			addSpriteToBuffer(thisChar);
			addCharToBuffer(' ');
		}
		addCharToBuffer((y / 10) % 10 + '0');
		addCharToBuffer(y % 10 + '0');
		newLine();
	}
	flush();
}
void start()
{
	printf("\33[?25l");
}
void stop()
{
	printf("\033[0");
	printf("\33[?25h");
}
void main()
{
	printf("\033[0");
	addFunctionStart(&start);
	addFunctionStop(&stop);
	addFunctionLoop(&loop);
	setFps(50);
	startGame();
	
}