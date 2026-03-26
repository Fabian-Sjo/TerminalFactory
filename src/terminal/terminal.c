#include "terminal.h"

#include <windows.h>

HANDLE stdOutHandle;
HANDLE stdInHandle;
CONSOLE_CURSOR_INFO consoleCursorInfo;


Key heldKeys[N_KEY_ROLLOVER];
int nrOfHeldKeys = 0;

void terminalInit()
{

	HANDLE stdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE stdInHandle = GetStdHandle(STD_INPUT_HANDLE);
	GetConsoleCursorInfo(stdOutHandle, &consoleCursorInfo);

	int consoleSettings = ENABLE_EXTENDED_FLAGS;
	if (!SetConsoleMode(stdInHandle, consoleSettings))
		exit(-1);
	consoleSettings = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
	if (!SetConsoleMode(stdInHandle, consoleSettings))
		exit(-2);
}
void terminalReset()
{
}
Key virtualKeyCodeToKey(WORD keyCode)
{
	switch (keyCode)
	{
	default:
		return KEY_A;
	}
}
InputEvent poolInput()
{
	INPUT_RECORD InputRecord;
	DWORD Events;
	int nrOfEvents;
	InputEvent inputEvent = {0};
	GetNumberOfConsoleInputEvents(stdInHandle, &nrOfEvents);

	if (nrOfEvents == 0)
		return;

	ReadConsoleInput(stdInHandle, &InputRecord, 1, &Events);

	switch (InputRecord.EventType)
	{
	case KEY_EVENT:
	{
		KEY_EVENT_RECORD eventRecord = InputRecord.Event.KeyEvent;
		static KeyEvent keyEvent;
		keyEvent.pressed = KEY_NONE;
		keyEvent.released = KEY_NONE;
		if (eventRecord.wVirtualKeyCode)
		{
			Key key = virtualKeyCodeToKey(eventRecord.wVirtualKeyCode);
			if (eventRecord.bKeyDown)
			{
				keyEvent.pressed = key;
				heldKeys[nrOfHeldKeys++] = key;
			}
			else
			{
				keyEvent.released = key;
				int isFound = FALSE;
				for (int i = 0; i < nrOfHeldKeys; i++)
				{
					if (heldKeys[i] == key)
						isFound = TRUE;
					if (isFound)
					{
						if (i == nrOfHeldKeys)
							heldKeys[i] = KEY_NONE;
						else
							heldKeys[i] = heldKeys[i + 1];
					}
				}
			}
		}
		for (int i = 0; i < nrOfHeldKeys; i++)
			keyEvent.held[i] = heldKeys[i];
		inputEvent.keyEvent = keyEvent;
		break;
	}
	case MOUSE_EVENT: // mouse input
		break;
	case WINDOW_BUFFER_SIZE_EVENT: // scrn buf. resizing
	case FOCUS_EVENT:			   // disregard focus events
	case MENU_EVENT:			   // disregard menu events
		break;
	default:
		break;
	}
}

void terminalDraw()
{
	FlushConsoleInputBuffer(stdInHandle);
}

void terminalSetTextColor255(Color color);
void terminalSetTextColor16(Color16 color);

int terminalIsKeyPressed(Key key)
{
}
Vector2Int terminalGetMousePos()
{
	LPPOINT posPoint;
	//GetCursorPos(posPoint);
	return (Vector2Int){posPoint->x, posPoint->y};
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
Vector2Int terminalGetCursor()
{
}

void terminalDrawChar(char character);
void terminalDrawUTF(char character[4]);
void terminalDrawText(char *character);

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