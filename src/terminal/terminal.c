#include "terminal.h"
#include <stdio.h>
#include <windows.h>
#pragma comment(lib, "User32.lib") // compile with User32.lib

HANDLE stdOutHandle;
HANDLE stdInHandle;
CONSOLE_CURSOR_INFO consoleCursorInfo;
typedef struct KeyQueue
{
	int maxNrOfKeys;
	Key keys[N_KEY_ROLLOVER];
	int nrOfKeys;

} KeyQueue;

KeyQueue pressedKeys = {.maxNrOfKeys = N_KEY_ROLLOVER, .nrOfKeys = 0};
KeyQueue heldKeys = {.maxNrOfKeys = N_KEY_ROLLOVER, .nrOfKeys = 0};
KeyQueue releasedKeys = {.maxNrOfKeys = N_KEY_ROLLOVER, .nrOfKeys = 0};
int queueContains(KeyQueue *queue, Key key)
{
	if (!queue->nrOfKeys)
		return FALSE;
	for (int i = 0; i <= queue->nrOfKeys; i++)
	{
		if (queue->keys[i] == key)
			return TRUE;
	}
	return FALSE;
}
void queueAdd(KeyQueue *queue, Key key)
{
	if (queueContains(queue, key))
		return;
	if (queue->nrOfKeys >= queue->maxNrOfKeys)
		return;
	queue->keys[queue->nrOfKeys++] = key;
}
void queueClear(KeyQueue *queue)
{
	queue->nrOfKeys = 0;
}
void queueRemove(KeyQueue *queue, Key key)
{
	int isFound = FALSE;
	for (int i = 0; i < queue->nrOfKeys; i++)
	{
		if (queue->keys[i] == key)
			isFound = TRUE;
		if (isFound)
		{
			queue->keys[i] = queue->keys[i + 1];
		}
	}
	if (isFound)
		queue->nrOfKeys--;
}
void queueAppendToQueue(KeyQueue *source, KeyQueue *destination)
{
	if (source->nrOfKeys == 0)
		return;
	for (int i = 0; i < source->nrOfKeys; i++)
	{
		queueAdd(destination, source->keys[i]);
	}
}

void terminalInit()
{

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
void poolInput()
{

	queueClear(&releasedKeys);
	queueAppendToQueue(&pressedKeys, &heldKeys);
	queueClear(&pressedKeys);

	DWORD numEvents = 0;
	GetNumberOfConsoleInputEvents(stdInHandle, &numEvents);
	KEY_EVENT_RECORD eventRecords[4];
	int nrOfRecords = 0;



	while (numEvents > 0)
	{
		INPUT_RECORD record;
		DWORD eventsRead;

		ReadConsoleInput(stdInHandle, &record, 1, &eventsRead);

		if (record.EventType == KEY_EVENT)
		{
			KEY_EVENT_RECORD event = record.Event.KeyEvent;
			eventRecords[nrOfRecords++] = event;
			Key key = virtualKeyCodeToKey(event.wVirtualKeyCode);
			if (event.bKeyDown)
			{
				if (!queueContains(&heldKeys, key))
				{

					queueAdd(&pressedKeys, key);
					queueAdd(&heldKeys, key);
				}
			}
			else
			{
				queueRemove(&heldKeys, key);
				queueAdd(&releasedKeys, key);
			}
		}

		numEvents--;
	}
}
int terminalIsKeyPressed(Key key)
{
	return queueContains(&pressedKeys, key);
}
int terminalIsKeyHeld(Key key)
{
	return queueContains(&heldKeys, key);
}
int terminalIsKeyReleased(Key key)
{
	printf("releasedNr:%d", releasedKeys.nrOfKeys);
	return queueContains(&releasedKeys, key);
}

void terminalDraw()
{
	FlushConsoleInputBuffer(stdInHandle);
}

void terminalSetTextColor255(Color color);
void terminalSetTextColor16(Color16 color);

Vector2Int terminalGetMousePos()
{
	LPPOINT posPoint;
	// GetCursorPos(posPoint);
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