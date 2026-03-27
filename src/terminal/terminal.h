#ifndef TERMINAL_H
#define TERMINAL_H
#include "../utils/vector2.h"
#include "../graphical/color.h"
#define N_KEY_ROLLOVER 8
typedef enum Key
{
	KEY_NONE,
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_ESC,
	KEY_SPACE,
	KEY_AMMOUNT
} Key;
typedef enum KeyState
{
	KEY_RELEASED = 0,
	KEY_JUST_RELEASED = -1,
	KEY_PRESSED = 1,
	KEY_JUST_PRESSED = 2,
} KeyState;

typedef enum Color16
{
	COLOR16_BLACK = 0,
	COLOR16_BLUE = 1,
	COLOR16_GREEN = 2,
	COLOR16_CYAN = 3,
	COLOR16_RED = 4,
	COLOR16_MAGENTA = 5,
	COLOR16_BROWN = 6,
	COLOR16_LIGHTGRAY = 7,
	COLOR16_DARKGRAY = 8,
	COLOR16_LIGHTBLUE = 9,
	COLOR16_LIGHTGREEN = 10,
	COLOR16_LIGHTCYAN = 11,
	COLOR16_LIGHTRED = 12,
	COLOR16_LIGHTMAGENTA = 13,
	COLOR16_YELLOW = 14,
	COLOR16_WHITE = 15
} Color16;

void terminalInit();
void terminalReset();

void poolInput();

void terminalSetTextColor(Color color);
void terminalSetTextColorBackground(Color color);
void terminalSetTextColor16(Color16 color);

KeyState terminalGetKeyState(Key key);
Vector2Int terminalGetMousePos();

void terminalSetCursorPos(Vector2Int pos);
void terminalSetCursorSize(int size);
void terminalSetCursorVisible(int visible);
Vector2Int terminalGetCursorPos();

void terminalDrawChar(char character);
void terminalDrawText(char *character);

void terminalDraw();

void terminalClear();

void terminalSetTitle(char *title);
#endif