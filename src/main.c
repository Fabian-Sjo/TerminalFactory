#include "systemIndependant.h"
#include "perlin.h"
#include "renderer.h"
#include "input.h"

#include <stdio.h>
#include <signal.h>

#include "gameLoop.h"
#define BOARD_WIDTH 66
#define BOARD_HEIGHT 33

int TERRAIN_PARTS[] = {SPRITE_WATER_DEEP, SPRITE_WATER, SPRITE_SAND, SPRITE_GRASS, SPRITE_STONE, SPRITE_SNOW};
int pos_x = 0;
int pos_y = 0;
float zoom = 0.1;
char board[BOARD_WIDTH][BOARD_HEIGHT];
int frame = 0;

void loop(long deltaTime)
{
	printf("delta time: %10dns\n", deltaTime);
	KeyEvent keyEvent = getKeyEvent();
	printf("Frame: %d  ", frame++);
	printf("Pos x: %-3d Pos y: %-3d", pos_x, pos_y);
	printf("pressed: %-10s released: %-10s held: %-10s ", keyToString(keyEvent.pressed), keyToString(keyEvent.released), keyToString(keyEvent.held));
	switch (keyEvent.pressed)
	{
	case KEY_Q:
		zoom += 0.1;
		break;
	case KEY_E:
		zoom -= 0.1;
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
	case KEY_ESC:
		stopGame();
		break;

	default:
		break;
	}

	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{

			int terrainIndex = perlin_Get2d(
								   (x - BOARD_WIDTH / 2) * zoom + pos_x,
								   (y - BOARD_HEIGHT / 2) * zoom + pos_y,
								   0.1, 1) *
							   (sizeof(TERRAIN_PARTS) / sizeof(TERRAIN_PARTS[0]));
			board[x][y] = TERRAIN_PARTS[terrainIndex];
			// board[x][y] = (x + pos_x) % 10 + ((y + pos_y) % 10) * 10;
		}
	}
	// printf("\033[2J"); // Clear entire screen
	// printf("\033[H");  // Move cursor to home position
	printf("\033[H"); // scroll back terminal
	
	// pos_x++;
	for (int y = 0; y < BOARD_HEIGHT; y++)
	{
		for (int x = 0; x < BOARD_WIDTH; x++)
		{
			
			char thisChar = board[x][y];
			
			// setColorFore(x+y);
			// setColorBack(x+y);
			// addCharToBuffer((y + pos_y) % 10 + '0');
			// addCharToBuffer(((x + pos_x) / 10) % 10 + '0');
			// addCharToBuffer(thisChar);
			addSpriteToBuffer(thisChar);
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
}
void stop()
{
	printf("\033[0");
	printf("\33[?25l"); // reset ansi
	printf("\033[2J");	// clear terminal
	printf("\033[H"); // scroll back terminal
	fflush(stdout);
}

void main()
{

	signal(SIGINT, stopGame);

	
	printf("\033[0");
	addFunctionStart(&start);
	addFunctionStop(&stop);
	addFunctionLoop(&loop);
	setFps(50);
	startGame();
}
/*
	  > - > - > - > - > - > - > - > - > - > - > - > - > - > - > - > - > - > - > - > -
A			  v
A			  v
A			  v
< < < < < < < <

*/
