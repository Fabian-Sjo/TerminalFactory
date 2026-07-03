#include "../src/utils/systemIndependant.h"
#include "../src/utils/map.h"
#include "../src/utils/vector2.h"
#include "../src/terminal/terminal.h"

#include "../src/graphical/renderer.h"
#include "../src/graphical/canvas.h"

#include "../src/sound/sound.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>

typedef struct Window Window;

struct Window
{
	Vector2Int position;
	Window *parent;
	Canvas *canvas;
	Vector2Int size;
};

Window screenWindow = {
	.position = {0, 0},
	.parent = NULL,
	.canvas = NULL,
	.size = {20, 20}};

Window canvasWindow = {
	.position = {10, 2},
	.parent = NULL,
	.canvas = NULL,
	.size = {35, 35}};

Window palleteWindow = {
	.parent = NULL,
	.canvas = NULL,
	.size = {20, 20}};


	
Sprite selectedSprite = {.icon = '@', {100, 100, 0}, COLOR_BLACK_CONST};
Sprite deletedSprite = {.icon = ' ', {0, 0, 0}, {0, 0, 0}};

char *iconPallete = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~;";

Vector2Int cursorPos;

Vector2Int localizeMousePos(Vector2Int mousePos, Window window)
{
	if (window.parent != NULL)
		mousePos = localizeMousePos(mousePos, *window.parent);

	Vector2Int scale = canvasGetDisplayScale(window.canvas);
	return (Vector2Int){
		(mousePos.x - window.position.x) / scale.x,
		(mousePos.y - window.position.y) / scale.y};
}
bool isMouseInBounds(Vector2Int mousePos, Window window)
{
	Vector2Int localMousePos = localizeMousePos(mousePos, window);
	return (localMousePos.x >= 0 &&
			localMousePos.y >= 0 &&
			localMousePos.x < window.size.x &&
			localMousePos.y < window.size.y);
}
void loop(double deltaTime)
{
	Vector2Int termsize = vecDivI(getTermSize(), (Vector2Int){1 + canvasGetDoubleSpaced(screenWindow.canvas), 1});
	termsize.y = canvasWindow.size.y + 10;
	Vector2Int canvasDisplaySize = {
		canvasWindow.size.x * (1 + (canvasGetDoubleSpaced(canvasWindow.canvas))),
		canvasWindow.size.y};

	Vector2Int screenMousePos = localizeMousePos(cursorPos, screenWindow);
	Vector2Int canvasMousePos = localizeMousePos(cursorPos, canvasWindow);

	canvasSetSize(screenWindow.canvas, termsize);
	screenWindow.size = termsize;

	terminalSetCursorPos((Vector2Int){0, 0});
	canvasFill(screenWindow.canvas, (Sprite){.icon = ' ', .colorBack = COLOR_BLACK, .colorFore = COLOR_WHITE});
	canvasCopyToCanvas(screenWindow.canvas, canvasWindow.position, canvasWindow.canvas, (Vector2Int){0, 0}, canvasWindow.size);

	NineRect nineRect = {
		{{(Sprite){'\\'}, (Sprite){'|'}, (Sprite){'/'}},
		 {(Sprite){'='}, (Sprite){'.'}, (Sprite){'='}},
		 {(Sprite){'/'}, (Sprite){'|'}, (Sprite){'\\'}}}};

	canvasDrawNineRect(screenWindow.canvas,
					   vecSubI(canvasWindow.position, (Vector2Int){1, 1}),
					   vecAddI(canvasDisplaySize, (Vector2Int){2, 2}),
					   nineRect, FILL_NONE);

	palleteWindow.position = (Vector2Int){canvasWindow.position.x, canvasWindow.position.y + canvasWindow.size.y + 1};
	palleteWindow.size.x = canvasWindow.size.x * (1 + canvasGetDoubleSpaced(canvasWindow.canvas));
	palleteWindow.size.y = 5;

	char iconIndex = -1;
	while ((iconPallete[++iconIndex]) != 0)
	{
		Vector2Int pos = {(iconIndex) % palleteWindow.size.x, (iconIndex) / palleteWindow.size.x};
		canvasSetSprite(screenWindow.canvas, vecAddI(palleteWindow.position, pos),
						(Sprite){
							.icon = iconPallete[iconIndex],
							.colorBack = COLOR_BLACK,
							.colorFore = COLOR_WHITE});
	}

	rendererDrawCanvas(screenWindow.canvas);
	terminalSetCursorVisible(false);
	rendererFlush();
	printf("fullScreen: %d canvasWindow.canvas %d\n",
		   canvasGetDoubleSpaced(screenWindow.canvas),
		   canvasGetDoubleSpaced(canvasWindow.canvas));
	printf("true mouse pos		:{%2d,%2d} [%d]\n",
		   cursorPos.x,
		   cursorPos.y);
	printf("screen mouse pos	:{%2d,%2d} [%d]\n",
		   screenMousePos.x,
		   screenMousePos.y,
		   isMouseInBounds(cursorPos, screenWindow));
	printf("canvasMousePos		:{%2d,%2d} [%d]\n",
		   canvasMousePos.x,
		   canvasMousePos.y,
		   isMouseInBounds(cursorPos, canvasWindow));
	printf("palleteMousePos		:{%2d,%2d} [%d]\n",
		   localizeMousePos(cursorPos, palleteWindow).x,
		   localizeMousePos(cursorPos, palleteWindow).y,
		   isMouseInBounds(cursorPos, palleteWindow));

	poolInput();
	cursorPos = vecDivI(terminalGetMousePos(), (Vector2Int){1, 1});
	if (isMouseInBounds(cursorPos, canvasWindow))
	{
		terminalSetCursorPos(
			vecMulI(
				vecDivI(
					cursorPos,
					canvasGetDisplayScale(canvasWindow.canvas)),
				canvasGetDisplayScale(canvasWindow.canvas)));

		terminalSetCursorVisible(true);
	}
	if (terminalGetKeyState(KEY_ESC) == KEY_JUST_PRESSED)
	{
		stopGame();
	};
	if (terminalGetKeyState(KEY_S))
	{
	};
	if (terminalGetKeyState(KEY_W))
	{
	};
	if (terminalGetKeyState(KEY_A))
	{
	};
	if (terminalGetKeyState(KEY_D))
	{
	};
	if (terminalGetKeyState(KEY_C) == KEY_JUST_PRESSED)
	{
		canvasSetDoubleSpaced(canvasWindow.canvas, !canvasGetDoubleSpaced(canvasWindow.canvas));
	};
	if (terminalGetKeyState(KEY_V) == KEY_JUST_PRESSED)
	{
		canvasSetDoubleSpaced(screenWindow.canvas, !canvasGetDoubleSpaced(screenWindow.canvas));
	};
	if (terminalGetKeyState(KEY_SPACE))
	{
	};
	if (terminalGetKeyState(KEY_MOUSE_1))
	{
		if (isMouseInBounds(cursorPos, canvasWindow))
		{

			canvasSetSprite(canvasWindow.canvas, canvasMousePos, selectedSprite);
		}
		else if (isMouseInBounds(cursorPos, palleteWindow))
		{
			Vector2Int localMousePos = localizeMousePos(cursorPos, palleteWindow);
			int charIndex = (localMousePos.x % palleteWindow.size.x + (localMousePos.y * palleteWindow.size.x));
			int palleteSize = 0;
			if (charIndex >= strlen(iconPallete))
				return;
			char selectedChar = iconPallete[charIndex];
			selectedSprite.icon = selectedChar;
			printf("%3d - %3c", charIndex, selectedChar);
		}
	};
	if (terminalGetKeyState(KEY_MOUSE_2))
	{
		canvasSetSprite(canvasWindow.canvas, canvasMousePos, deletedSprite);
	};
	// terminalDrawCanvas(gameData->canvasWindow.canvas);
	// terminalDraw();
}

void start()
{

	printf("\033[2J");	// clear terminal
	printf("\33[?25l"); // reset ansi
	canvasWindow.parent = &screenWindow;
	palleteWindow.parent = &screenWindow;

	canvasWindow.canvas = canvasNew(canvasWindow.size);
	canvasSetDoubleSpaced(canvasWindow.canvas, true);
	screenWindow.canvas = canvasNew(screenWindow.size);
	canvasSetDoubleSpaced(screenWindow.canvas, false);

	terminalSetCursorVisible(true);
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
