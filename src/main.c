#include "systemIndependant.h"
#include "perlin.h"
#include "renderer.h"
#include <conio.h>

#define BOARD_WIDTH 60
#define BOARD_HEIGHT 50

int TERRAIN_PARTS[] = {SPRITE_WATER_DEEP,SPRITE_WATER, SPRITE_SAND, SPRITE_GRASS};



int main()
{
	printf("\033[0");
	int pos_x = 10;
	int pos_y = 10;

	char board[BOARD_WIDTH][BOARD_HEIGHT];
	int frame = 0;

	while (1)
	{
		if (_kbhit())
		{
			char c = _getch();

			if (c == 'q')
				break;
			if (c == 's')
				pos_y++;
			if (c == 'd')
				pos_x++;
			if (c == 'w')
				pos_y--;
			if (c == 'a')
				pos_x--;

			//printf("Pressed: %c\n", c);
			//printf("x: %d\n", pos_x);
			//printf("y: %d\n", pos_y);
		}
		for (int x = 0; x < BOARD_WIDTH; x++)
		{
			for (int y = 0; y < BOARD_HEIGHT; y++)
			{
				int terrainIndex = perlin2d(x + pos_x, y + pos_y, 0.1, 1) * (sizeof(TERRAIN_PARTS) / sizeof(TERRAIN_PARTS[0]));
				board[x][y] = TERRAIN_PARTS[terrainIndex];
				// board[x][y] = (x + pos_x) % 10 + ((y + pos_y) % 10) * 10;
			}
		}
		// printf("\033[2J"); // Clear entire screen
		// printf("\033[H");  // Move cursor to home position
		printf("\033[H"); // scroll back terminal
		// printf("\e[2J"); // clear terminal
		printf("Frame %d\n", frame++);
		pos_x++;
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			for (int x = 0; x < BOARD_WIDTH; x++)
			{

				char thisChar = board[x][y];

				//setColorFore(x+y);
				//setColorBack(x+y);
				//addCharToBuffer(y % 10 + '0');
				//addCharToBuffer((x / 10) % 10 + '0');
				//addCharToBuffer(thisChar);
				addSpriteToBuffer(thisChar);
				addCharToBuffer(' ');
			}
			addCharToBuffer((y / 10) % 10 + '0');
			addCharToBuffer(y % 10 + '0');
			newLine();
		}
		flush();
		sleep(16);
	}
}