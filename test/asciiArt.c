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
	void (*render)(Window window, Canvas *canvas);
	Vector2Int scale;
	Vector2Int size;
};

Sprite selectedSprite = {.icon = '@', {100, 100, 0}, COLOR_BLACK_CONST};
Sprite deletedSprite = {.icon = ' ', {0, 0, 0}, {0, 0, 0}};

char *iconPallete = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~;";

Vector2Int cursorPos;
Canvas *artCanvas;
Canvas *screenCanvas;

void renderCanvas(Window window, Canvas *canvas)
{
	canvasCopyToCanvas(
		canvas,
		window.position,
		artCanvas,
		(Vector2Int){0, 0},
		vecDivI(window.size, window.scale));
}
void renderPallete(Window window, Canvas *canvas)
{
	char iconIndex = -1;
	while ((iconPallete[++iconIndex]) != 0)
	{
		Vector2Int pos = {(iconIndex) % window.size.x, (iconIndex) / window.size.x};
		canvasSetSprite(canvas, vecAddI(window.position, pos),
						(Sprite){
							.icon = iconPallete[iconIndex],
							.colorBack = COLOR_BLACK,
							.colorFore = COLOR_WHITE});
	}
}
void renderUI(Window window, Canvas *canvas)
{
	NineRect nineRect = {
		{{(Sprite){'\\'}, (Sprite){'|'}, (Sprite){'/'}},
		 {(Sprite){'='}, (Sprite){'.'}, (Sprite){'='}},
		 {(Sprite){'/'}, (Sprite){'|'}, (Sprite){'\\'}}}};

	canvasDrawNineRect(canvas,
					   vecSubI(window.position, (Vector2Int){0, 0}),
					   vecAddI(window.size, (Vector2Int){0, 0}),
					   nineRect, FILL_NONE);
}
Window screenWindow = {
	.position = {0, 0},
	.parent = NULL,
	.scale = {1, 1},
	.render = &renderUI,
	.size = {20, 20}};

Window canvasWindow = {
	.position = {10, 2},
	.parent = NULL,
	.scale = {1, 1},
	.render = &renderCanvas,
	.size = {35, 35}};

Window palleteWindow = {
	.parent = NULL,
	.scale = {1, 1},
	.render = &renderPallete,
	.size = {20, 20}};

Vector2Int localizeMousePos(Vector2Int mousePos, Window window)
{
	if (window.parent != NULL)
		mousePos = localizeMousePos(mousePos, *window.parent);

	Vector2Int scale = window.scale;
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
	Vector2Int termsize = vecDivI(getTermSize(), (Vector2Int){1 + canvasGetDoubleSpaced(screenCanvas), 1});
	termsize.y = canvasWindow.size.y + 10;
	Vector2Int canvasDisplaySize = {
		canvasWindow.size.x * (1 + (canvasGetDoubleSpaced(artCanvas))),
		canvasWindow.size.y};

	Vector2Int screenMousePos = localizeMousePos(cursorPos, screenWindow);
	Vector2Int canvasMousePos = localizeMousePos(cursorPos, canvasWindow);

	canvasSetSize(screenCanvas, termsize);
	screenWindow.size = termsize;

	palleteWindow.position = (Vector2Int){canvasWindow.position.x, canvasWindow.position.y + canvasWindow.size.y + 1};
	palleteWindow.size.x = canvasWindow.size.x;
	palleteWindow.size.y = 5;

	terminalSetCursorPos((Vector2Int){0, 0});
	canvasFill(screenCanvas, (Sprite){.icon = '*', .colorBack = COLOR_BLACK, .colorFore = COLOR_WHITE});

	screenWindow.render(screenWindow, screenCanvas);
	canvasWindow.render(canvasWindow, screenCanvas);
	palleteWindow.render(palleteWindow, screenCanvas);

	rendererDrawCanvas(screenCanvas);

	terminalSetCursorVisible(false);
	rendererFlush();
	printf("fullScreen: %d artCanvas %d\n",
		   canvasGetDoubleSpaced(screenCanvas),
		   canvasGetDoubleSpaced(artCanvas));
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
	printf("widnth %d", canvasWindow.size.x);
	poolInput();
	cursorPos = vecDivI(terminalGetMousePos(), (Vector2Int){1, 1});
	if (isMouseInBounds(cursorPos, canvasWindow))
	{
		terminalSetCursorPos(
			vecMulI(
				vecDivI(
					cursorPos,
					canvasGetDisplayScale(artCanvas)),
				canvasGetDisplayScale(artCanvas)));

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
		canvasSetDoubleSpaced(artCanvas, !canvasGetDoubleSpaced(artCanvas));
		canvasWindow.scale = canvasGetDisplayScale(artCanvas);
		canvasWindow.size = vecMulI(canvasGetSize(artCanvas), canvasWindow.scale);
	};
	if (terminalGetKeyState(KEY_V) == KEY_JUST_PRESSED)
	{
		canvasSetDoubleSpaced(screenCanvas, !canvasGetDoubleSpaced(screenCanvas));
		screenWindow.scale = canvasGetDisplayScale(screenCanvas);
		screenWindow.size = vecMulI(canvasGetSize(screenCanvas), screenWindow.scale);
	};
	if (terminalGetKeyState(KEY_SPACE))
	{
	};
	if (terminalGetKeyState(KEY_MOUSE_1))
	{
		if (isMouseInBounds(cursorPos, canvasWindow))
		{

			canvasSetSprite(artCanvas, canvasMousePos, selectedSprite);
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
		canvasSetSprite(artCanvas, canvasMousePos, deletedSprite);
	};
	// terminalDrawCanvas(gameData->artCanvas);
	// terminalDraw();
}

void start()
{

	printf("\033[2J");	// clear terminal
	printf("\33[?25l"); // reset ansi
	canvasWindow.parent = &screenWindow;
	palleteWindow.parent = &screenWindow;

	artCanvas = canvasNew(canvasWindow.size);
	canvasSetDoubleSpaced(artCanvas, true);
	canvasWindow.scale = canvasGetDisplayScale(artCanvas);
	screenCanvas = canvasNew(screenWindow.size);
	canvasSetDoubleSpaced(screenCanvas, false);

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
