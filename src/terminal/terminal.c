#include "terminal.h"
#include <stdio.h>
#include <windows.h>

#pragma comment(lib, "User32.lib") // compile with User32.lib

HANDLE stdOutHandle;
HANDLE stdInHandle;
CONSOLE_CURSOR_INFO consoleCursorInfo;

Vector2Int mousePos;

KeyState keys[KEY_AMMOUNT];

Color currentForeColor = {0, 0, 0};
Color currentBackColor = {0, 0, 0};

void terminalInit()
{
	SetConsoleOutputCP(CP_UTF8);
	stdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	stdInHandle = GetStdHandle(STD_INPUT_HANDLE);
	GetConsoleCursorInfo(stdOutHandle, &consoleCursorInfo);
	DWORD mode;
	GetConsoleMode(stdInHandle, &mode);

	mode =
		ENABLE_EXTENDED_FLAGS |
		ENABLE_WINDOW_INPUT |
		ENABLE_MOUSE_INPUT |
		ENABLE_PROCESSED_INPUT;
	if (!SetConsoleMode(stdInHandle, mode))
		exit(-1);
}
void terminalReset()
{
}
int keyToCode(Key key)
{

	switch (key)
	{
	case KEY_A:
		return 'A';
	case KEY_B:
		return 'B';
	case KEY_C:
		return 'C';
	case KEY_D:
		return 'D';
	case KEY_E:
		return 'E';
	case KEY_F:
		return 'F';
	case KEY_G:
		return 'G';
	case KEY_H:
		return 'H';
	case KEY_I:
		return 'I';
	case KEY_J:
		return 'J';
	case KEY_K:
		return 'K';
	case KEY_L:
		return 'L';
	case KEY_M:
		return 'M';
	case KEY_N:
		return 'N';
	case KEY_O:
		return 'O';
	case KEY_P:
		return 'P';
	case KEY_Q:
		return 'Q';
	case KEY_R:
		return 'R';
	case KEY_S:
		return 'S';
	case KEY_T:
		return 'T';
	case KEY_U:
		return 'U';
	case KEY_V:
		return 'V';
	case KEY_W:
		return 'W';
	case KEY_X:
		return 'X';
	case KEY_Y:
		return 'Y';
	case KEY_Z:
		return 'Z';
	case KEY_SPACE:
		return VK_SPACE;
	case KEY_ESC:
		return VK_ESCAPE;
	default:
		return 0;
	}
}
void poolInput()
{

	for (int buttonIndex = 0; buttonIndex < KEY_AMMOUNT; buttonIndex++)
	{
		int keyCode = keyToCode(buttonIndex);
		KeyState newState = (GetKeyState(keyCode) & 0b10000000) ? KEY_PRESSED : KEY_RELEASED;
		KeyState oldState = keys[buttonIndex];
		KeyState state;
		if (newState == KEY_PRESSED)
			if (oldState != KEY_PRESSED && oldState != KEY_JUST_PRESSED)
				state = KEY_JUST_PRESSED;
			else
				state = KEY_PRESSED;
		else if (oldState != KEY_RELEASED && oldState != KEY_JUST_RELEASED)
			state = KEY_JUST_RELEASED;
		else
			state = KEY_RELEASED;
		keys[buttonIndex] = state;
	}
	INPUT_RECORD rec;
	DWORD read;
	int nrOfEvents;
	GetNumberOfConsoleInputEvents(stdInHandle, &nrOfEvents);

	if (nrOfEvents == 0)
		return;
	ReadConsoleInput(stdInHandle, &rec, 1, &read);
	if (rec.EventType == MOUSE_EVENT)
	{
		mousePos = (Vector2Int){rec.Event.MouseEvent.dwMousePosition.X,
								rec.Event.MouseEvent.dwMousePosition.Y};
	}
}
KeyState terminalGetKeyState(Key key)
{
	return keys[key];
}
void terminalDraw()
{
	FlushConsoleInputBuffer(stdInHandle);
}

void terminalSetTextColor(Color colorForeground)
{
	if (colorEquals(colorForeground, currentForeColor) || colorEquals(colorForeground, COLOR_TRANSPARENT))
		return;
	currentForeColor.R = colorForeground.R;
	currentForeColor.G = colorForeground.G;
	currentForeColor.B = colorForeground.B;
	updateColor();
}
void terminalSetTextColorBackground(Color colorBackground)
{
	if (colorEquals(colorBackground, currentBackColor) || colorEquals(colorBackground, COLOR_TRANSPARENT))
		return;
	currentBackColor.R = colorBackground.R;
	currentBackColor.G = colorBackground.G;
	currentBackColor.B = colorBackground.B;
	updateColor();
}
void updateColor()
{
	char strColor[48];
	// snprintf(strColor, sizeof(strColor), "\033[0m");
	snprintf(strColor, sizeof(strColor), "\033[38;2;%d;%d;%dm", currentForeColor.R, currentForeColor.G, currentForeColor.B);
	addStrToBuffer(strColor);
	snprintf(strColor, sizeof(strColor), "\033[48;2;%d;%d;%dm", currentBackColor.R, currentBackColor.G, currentBackColor.B);
	addStrToBuffer(strColor);
}
void terminalSetTextColor16(Color16 color)
{
	SetConsoleTextAttribute(stdOutHandle, color);
}

Vector2Int terminalGetMousePos()
{
	return mousePos;
	/*POINT pos;
	HWND hwnd = GetConsoleWindow();
	GetCursorPos(&pos);
	printf("Screen: %ld %ld\n", pos.x, pos.y);

	ScreenToClient(hwnd, &pos);

	printf("Client: %ld %ld\n", pos.x, pos.y);
	return (Vector2Int){pos.x, pos.y};*/
}
void terminalSetCursorPos(Vector2Int pos)
{
	COORD coord;
	coord.X = pos.x;
	coord.Y = pos.y;
	SetConsoleCursorPosition(stdOutHandle, coord);
	return;
}
void terminalSetCursorSize(int size)
{
	consoleCursorInfo.dwSize = size;
	SetConsoleCursorInfo(stdOutHandle, &consoleCursorInfo);
}
void terminalSetCursorVisible(int visible)
{
	consoleCursorInfo.bVisible = visible;
	SetConsoleCursorInfo(stdOutHandle, &consoleCursorInfo);
}
Vector2Int terminalGetCursorPos()
{
	POINT coord;
	GetCursorPos(&coord);
	return (Vector2Int){coord.x, coord.y};
}

void terminalDrawChar(char character)
{
	printf("%c", character);
}
void terminalDrawText(char *text)
{
	printf("%s", text);
}

void terminalClear()
{
	COORD coordScreen = {0, 0};
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;

	GetConsoleScreenBufferInfo(stdOutHandle, &csbi);
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	FillConsoleOutputCharacter(stdOutHandle, TEXT(' '), dwConSize, coordScreen, &cCharsWritten);
	GetConsoleScreenBufferInfo(stdOutHandle, &csbi);
	FillConsoleOutputAttribute(stdOutHandle, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
	SetConsoleCursorPosition(stdOutHandle, coordScreen);
	return;
}

void terminalSetTitle(char *title);