#ifndef TERMINAL_H
#define TERMINAL_H
#include <stdbool.h>
#include "../utils/vector2.h"
#include "../graphical/color.h"
#include "../graphical/canvas.h"

int printCalls;

typedef enum Key
{
	KEY_NONE,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
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
	KEY_MOUSE_1,
	KEY_MOUSE_2,
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

// Initializes the terminal subsystem and prepares it for drawing and input.
void terminalInit();
// Resets the terminal to a clean state after use.
void terminalReset();

// Polls and updates the current input state from the terminal.
void poolInput();

// Sets the active foreground text color.
void terminalSetTextColor(Color color);
// Sets the active background text color.
void terminalSetTextColorBackground(Color color);
// Sets the active color using the 16-color palette.
void terminalSetTextColor16(Color16 color);

// Returns the current input state for the supplied key. Valid keys are those defined by the Key enum.
KeyState terminalGetKeyState(Key key);
// Returns the current mouse position in terminal coordinates, as reported by the console input stream.
Vector2Int terminalGetMousePos();

// Shows or hides the terminal mouse cursor.
void terminalSetMouseVisibility(bool visable);

// Sets the cursor position to the provided coordinates.
void terminalSetCursorPos(Vector2Int pos);
// Sets the cursor size as a percentage value supported by the terminal; values are passed straight to the console API.
void terminalSetCursorSize(int size);
// Shows or hides the terminal cursor.
void terminalSetCursorVisible(int visible);
// Returns the current cursor position.
Vector2Int terminalGetCursorPos();

// Draws the provided canvas to the terminal.
void terminalDrawCanvas(Canvas *canvas);
// Draws a single character at the current cursor position.
void terminalDrawChar(char character);
// Draws a null-terminated text string at the current cursor position.
void terminalDrawText(char *character);

// Flushes the pending terminal draw operations.
void terminalDraw();

// Clears the terminal display.
void terminalClear();

// Sets the terminal window title.
void terminalSetTitle(char *title);

void terminalSetConsoleFont(const wchar_t *fontName, short size);
#endif