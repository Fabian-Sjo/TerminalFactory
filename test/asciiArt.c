#include "../src/utils/systemIndependant.h"
#include "../src/utils/map.h"
#include "../src/utils/vector2.h"
#include "../src/utils/ADT.h"
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
	// the thing the window is contained id
	Window *parent;
	// used to set positions
	Window *anchor;
	void (*render)(Window *window, Canvas *canvas);
	// returns true if the input is consumed
	bool (*mouse)(Window *window, Vector2Int localMousePos, Vector2Int globalMousePos);
	void (*update)(Window *windows);
	Vector2Int scale;
	Vector2Int size;
	int z;
};

Sprite selectedSprite = {.icon = '@', {100, 100, 0}, COLOR_BLACK_CONST};
Sprite deletedSprite = {.icon = ' ', {0, 0, 0}, {0, 0, 0}};

char *iconPallete = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~;";
LinkedList *windows;

Canvas *artCanvas;
Canvas *screenCanvas;
void canvasUpdate(Window *window);
void canvasRender(Window *window, Canvas *canvas);
bool canvasMouse(Window *window, Vector2Int localMousePos, Vector2Int globalMousePos);

void palleteUpdate(Window *window);
void palleteRender(Window *window, Canvas *canvas);
bool palleteMouse(Window *window, Vector2Int localMousePos, Vector2Int globalMousePos);

void colorUpdate(Window *window);
void colorRender(Window *window, Canvas *canvas);
bool colorMouse(Window *window, Vector2Int localMousePos, Vector2Int globalMousePos);

void screenUpdate(Window *window);
void screenRender(Window *window, Canvas *canvas);

Window screenWindow = {
	.position = {0, 0},
	.parent = NULL,
	.scale = {1, 1},
	.render = &screenRender,
	.update = &screenUpdate,
	.size = {20, 20},
	.z = 2};

Window canvasWindow = {
	.position = {10, 2},
	.parent = &screenWindow,
	.scale = {1, 1},
	.render = &canvasRender,
	.mouse = &canvasMouse,
	.update = &canvasUpdate,
	.size = {4, 4},
	.z = 0};

Window palleteWindow = {
	.parent = &screenWindow,
	.anchor = &canvasWindow,
	.scale = {1, 1},
	.render = &palleteRender,
	.mouse = &palleteMouse,
	.update = &palleteUpdate,
	.size = {20, 20},
	.z = 1};

Window colorWindow = {
	.parent = &screenWindow,
	.anchor = &canvasWindow,
	.scale = {1, 1},
	.render = &colorRender,
	.mouse = &colorMouse,
	.update = &colorUpdate,
	.size = {9, 35},
	.z = 7};

void canvasRender(Window *window, Canvas *canvas)
{
	canvasCopyToCanvas(
		canvas,
		window->position,
		artCanvas,
		(Vector2Int){0, 0},
		vecDivI(window->size, window->scale));
	NineRect nineRect = {
		{{(Sprite){'\\'}, (Sprite){'|'}, (Sprite){'/'}},
		 {(Sprite){'='}, (Sprite){'.'}, (Sprite){'='}},
		 {(Sprite){'/'}, (Sprite){'|'}, (Sprite){'\\'}}}};

	canvasDrawNineRect(canvas,
					   vecSubI(window->position, (Vector2Int){1, 1}),
					   vecAddI(window->size, (Vector2Int){2, 2}),
					   nineRect, FILL_NONE);
}
bool canvasMouse(Window *window, Vector2Int localMousePos, Vector2Int globalMousePos)
{
	terminalSetCursorPos(
		vecMulI(
			vecDivI(
				globalMousePos,
				canvasGetDisplayScale(artCanvas)),
			canvasGetDisplayScale(artCanvas)));

	terminalSetCursorVisible(true);
	if (terminalGetKeyState(KEY_MOUSE_1))
	{
		canvasSetSprite(artCanvas, localMousePos, selectedSprite);
	}
	else if (terminalGetKeyState(KEY_MOUSE_2))
	{
		canvasSetSprite(artCanvas, localMousePos, deletedSprite);
	}
	return true;
};
void canvasUpdate(Window *window)
{
}

void palleteRender(Window *window, Canvas *canvas)
{
	char iconIndex = -1;
	while ((iconPallete[++iconIndex]) != 0)
	{
		Vector2Int pos = {(iconIndex) % window->size.x, (iconIndex) / window->size.x};
		canvasSetSprite(canvas, vecAddI(window->position, pos),
						(Sprite){
							.icon = iconPallete[iconIndex],
							.colorBack = COLOR_BLACK,
							.colorFore = COLOR_WHITE});
	}
}
bool palleteMouse(Window *window, Vector2Int localMousePos, Vector2Int globalMousePos)
{
	if (!terminalGetKeyState(KEY_MOUSE_1))
		return false;
	int charIndex = (localMousePos.x % palleteWindow.size.x + (localMousePos.y * palleteWindow.size.x));
	int palleteSize = 0;
	if (charIndex >= strlen(iconPallete))
		return false;
	char selectedChar = iconPallete[charIndex];
	selectedSprite.icon = selectedChar;
	return true;
};
void palleteUpdate(Window *window)
{
	window->position = (Vector2Int){
		window->anchor->position.x,
		window->anchor->position.y + window->anchor->size.y};
	window->size.x = window->anchor->size.x;
	window->size.y = 5;
}

void screenRender(Window *window, Canvas *canvas)
{
	NineRect nineRect = {
		{{(Sprite){'\\'}, (Sprite){'|'}, (Sprite){'/'}},
		 {(Sprite){'='}, (Sprite){'.'}, (Sprite){'='}},
		 {(Sprite){'/'}, (Sprite){'|'}, (Sprite){'\\'}}}};

	canvasDrawNineRect(canvas,
					   vecSubI(window->position, (Vector2Int){0, 0}),
					   vecAddI(window->size, (Vector2Int){0, 0}),
					   nineRect, FILL_NONE);
}
void screenUpdate(Window *window)
{
	Vector2Int termsize = vecDivI(getTermSize(), (Vector2Int){1 + canvasGetDoubleSpaced(screenCanvas), 1});
	termsize.y = canvasWindow.size.y + 10;

	canvasSetSize(screenCanvas, termsize);
	window->size = termsize;
}

void colorRender(Window *window, Canvas *canvas)
{
	int colorBarStart = 1;
	int colorBarHeight = 32;
	for (size_t x = 0; x < 3; x++)
	{
		Color color = {
			0,
			0,
			0};
		int value = 0;
		if (x == 0)
		{
			value = selectedSprite.colorFore.R;
			color.R = value;
		}
		if (x == 1)
		{
			value = selectedSprite.colorFore.G;
			color.G = value;
		}
		if (x == 2)
		{
			value = selectedSprite.colorFore.B;
			color.B = value;
		}
		if (value == 255)
			value++;
		int scaledValue = (value * colorBarHeight) / 256;

		for (size_t y = 0; y < colorBarHeight + 1; y++)
		{
			Sprite sprite = {
				.icon = '|',
				.colorFore = color};
			if (colorBarHeight - y - 1 < scaledValue)
				sprite.icon = 'I';
			if (colorBarHeight - y == scaledValue)
				sprite.icon = '0';
			canvasSetSprite(canvas,
							vecAddI(window->position, (Vector2Int){(x + 1) * 2, y + colorBarStart}), sprite);
		}
	}
	struct HexColor hex = colorAsHex(selectedSprite.colorFore);
	for (size_t i = 0; i < 7; i++)
	{
		Sprite sprite = {
			.icon = hex.hex[i],
			.colorFore = selectedSprite.colorFore};
		canvasSetSprite(canvas,
						vecAddI(window->position, (Vector2Int){i + 1, colorBarStart + colorBarHeight}), sprite);
	}

	// canvasDrawRectangle(canvas,vecAddI(window->position, (Vector2Int){2, colorBarHeight + colorBarStart + 1}),(Vector2Int){5, 3},(Sprite){	.icon = '+',	.colorFore =		selectedSprite.colorFore},FILL_ALL);

	NineRect nineRect = {
		{{(Sprite){'\\'}, (Sprite){' '}, (Sprite){'/'}},
		 {(Sprite){'='}, (Sprite){'.'}, (Sprite){'='}},
		 {(Sprite){'/'}, (Sprite){' '}, (Sprite){'\\'}}}};
	// canvasDrawNineRect(canvas,vecSubI(window->position, (Vector2Int){0, 0}),vecAddI(window->size, (Vector2Int){0, 0}),nineRect, FILL_NONE);
}
bool colorMouse(Window *window, Vector2Int localMousePos, Vector2Int globalMousePos)
{
	if (!terminalGetKeyState(KEY_MOUSE_1))
		return false;
	int selectedIndex = -1;
	int selectedHeight = -1;
	int colorBarStart = 1;
	int colorBarHeight = 32;
	for (size_t x = 0; x < 3; x++)
	{
		Vector2Int topOfBar = {(x + 1) * 2, colorBarStart};
		Vector2Int botOfBar = {(x + 1) * 2, colorBarStart + colorBarHeight};
		bool isInRect = isPointInRect(localMousePos, topOfBar, botOfBar);
		if (isInRect)
		{
			selectedIndex = x;
			selectedHeight = colorBarHeight - localMousePos.y + colorBarStart;
		}
	}
	if (selectedIndex == -1)
		return false;
	int scaledValue = (selectedHeight);
	// int scaledValue = (value * colorBarHeight) / 256;
	int value = (scaledValue * 256) / colorBarHeight;
	if (value == 256)
		value--;
	if (selectedIndex == 0)
	{
		selectedSprite.colorFore.R = value;
	}
	if (selectedIndex == 1)
	{
		selectedSprite.colorFore.G = value;
	}
	if (selectedIndex == 2)
	{
		selectedSprite.colorFore.B = value;
	}
	printf("bar %2d selection height %3d value: %3d", selectedIndex, selectedHeight, value);
	return true;
}
void colorUpdate(Window *window)
{
	window->position =
		vecAddI(
			window->anchor->position,
			(Vector2Int){window->anchor->size.x, 0});
}

static int compareWindows(void *newItem, void *oldItem)
{
	Window *a = *(Window **)newItem;
	Window *b = *(Window **)oldItem;
	return a->z - b->z;
}

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
	poolInput();
	int nrOfWindows = windows->nrOfElements;
	for (size_t i = 0; i < nrOfWindows; i++)
	{
		Window *thisWindow = *(Window **)linkedListGet(windows, i);
		if (thisWindow->update != NULL)
			thisWindow->update(thisWindow);
	}
	Vector2Int cursorPos = vecDivI(terminalGetMousePos(), (Vector2Int){1, 1});
	// cursorPos = (Vector2Int){21, 32};
	int mouseConsumer = -1;
	for (size_t i = 0; i < nrOfWindows; i++)
	{
		Window *thisWindow = *(Window **)linkedListGet(windows, nrOfWindows - 1 - i);
		// printf("window %d ", (thisWindow)->z);
		if (!isMouseInBounds(cursorPos, *thisWindow))
			continue;
		if (thisWindow->mouse == NULL)
			continue;
		Vector2Int localMousePos = localizeMousePos(cursorPos, *thisWindow);
		bool wasConsumed = thisWindow->mouse(thisWindow, localMousePos, cursorPos);

		if (wasConsumed)
		{
			mouseConsumer = thisWindow->z;
			break;
		}
	}

	if (isMouseInBounds(cursorPos, canvasWindow))
	{
	}
	if (terminalGetKeyState(KEY_ESC) == KEY_JUST_PRESSED)
	{
		stopGame();
	};
	if (terminalGetKeyState(KEY_S))
	{
		printf("\033[2J"); // clear terminal
		char *canvasString = canvasToString(artCanvas, true);
		pasteToClipboard(canvasString);
		free(canvasString);
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
		// canvasSetDoubleSpaced(screenCanvas, !canvasGetDoubleSpaced(screenCanvas));
		// screenWindow.scale = canvasGetDisplayScale(screenCanvas);
	};
	if (terminalGetKeyState(KEY_SPACE))
	{
	};

	terminalSetCursorPos((Vector2Int){0, 0});
	canvasFill(screenCanvas, (Sprite){.icon = ' ', .colorBack = COLOR_BLACK, .colorFore = COLOR_WHITE});

	for (size_t i = 0; i < nrOfWindows; i++)
	{
		Window *thisWindow = *(Window **)linkedListGet(windows, i);
		printf("%d", (thisWindow)->z);
		thisWindow->render(thisWindow, screenCanvas);
	}

	rendererDrawCanvas(screenCanvas);
	terminalSetCursorVisible(false);
	rendererFlush();

	printf("fullScreen: %d artCanvas %d\n",
		   canvasGetDoubleSpaced(screenCanvas),
		   canvasGetDoubleSpaced(artCanvas));
	printf("true mouse pos		:{%2d,%2d} [%d]\n",
		   cursorPos.x,
		   cursorPos.y);
	printf("palleteMousePos		:{%2d,%2d} [%d]\n",
		   localizeMousePos(cursorPos, palleteWindow).x,
		   localizeMousePos(cursorPos, palleteWindow).y,
		   isMouseInBounds(cursorPos, palleteWindow));
	printf("mouseConsumer		:{%2d\n",
		   mouseConsumer);

	// terminalDrawCanvas(gameData->artCanvas);
	// terminalDraw();
}

void start()
{
	windows = linkedListCreate(sizeof(Window *));

	Window *cw = &canvasWindow;
	Window *cow = &colorWindow;
	Window *pw = &palleteWindow;
	Window *sw = &screenWindow;
	linkedListinsertSorted(windows, &cw, compareWindows);
	linkedListinsertSorted(windows, &cow, compareWindows);
	linkedListinsertSorted(windows, &pw, compareWindows);
	linkedListinsertSorted(windows, &sw, compareWindows);

	printf("\033[2J");	// clear terminal
	printf("\33[?25l"); // reset ansi

	artCanvas = canvasNew(canvasWindow.size);
	canvasSetDoubleSpaced(artCanvas, true);
	canvasWindow.scale = canvasGetDisplayScale(artCanvas);
	canvasWindow.size = vecMulI(canvasGetSize(artCanvas), canvasWindow.scale);

	screenCanvas = canvasNew(screenWindow.size);
	canvasSetDoubleSpaced(screenCanvas, false);
	screenWindow.scale = canvasGetDisplayScale(screenCanvas);
	screenWindow.size = vecMulI(canvasGetSize(screenCanvas), screenWindow.scale);

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
