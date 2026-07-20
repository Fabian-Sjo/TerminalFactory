#include <stdlib.h>
#include <stdio.h>

#include "sprite.h"
#include "color.h"
#include "renderer.h"
#include "canvas.h"
#include "../terminal/terminal.h"

char *buffer = NULL;
int nrOfElements = 0;
int bufferSize = 2;

Color currentForeColor = {0, 0, 0};
Color currentBackColor = {0, 0, 0};

void newLine();

void addCharToBuffer(char c);

void addStrToBuffer(char *str);

void setColorFore(Color colorForeground);

void setColorBack(Color colorBackground);
void updateColor();
void flush();

void newLine()
{
	addCharToBuffer('\n');
}

void rendererAddUTF8ToBuffer(UTF8 c)
{

	int nrOfChar = UTFByteLength(c);
	for (size_t i = 0; i < nrOfChar; i++)
	{
		addCharToBuffer(c.data[i]);
	}
}
void addCharToBuffer(char c)
{
	if (bufferSize - 3 <= nrOfElements)
	{
		bufferSize *= 2;
		buffer = realloc(buffer, bufferSize);
		if (!buffer)
		{
			perror("realloc");
			exit(1);
		}
	}

	*(buffer + nrOfElements) = c;
	nrOfElements++;

	*(buffer + (nrOfElements + 1)) = '\0'; // Add null terminator
}
void addSpriteToBuffer(Sprite sprite)
{
	if (!colorEquals(sprite.colorFore, currentForeColor) && !colorEquals(sprite.colorFore, COLOR_TRANSPARENT))
	{
		setColorFore((sprite.colorFore));
	}
	if (!colorEquals(sprite.colorBack, currentBackColor) && !colorEquals(sprite.colorBack, COLOR_TRANSPARENT))
	{
		setColorBack((sprite.colorBack));
	}
	UTF8 c = sprite.icon;
	if (c.data[0] == '\0')
		c.data[0] = ' ';
	rendererAddUTF8ToBuffer(c);
}
void rendererDrawCanvas(Canvas *canvas)
{
	Vector2Int size = vecMulI(canvasGetSize(canvas), (Vector2Int){1 + canvasGetDoubleSpaced(canvas), 1});
	for (int y = 0; y < size.y; y++)
	{
		for (int x = 0; x < size.x; x++)
		{
			addSpriteToBuffer(canvasGetSprite(canvas, (Vector2Int){x, y}));
		}
		newLine();
	}
}

void addStrToBuffer(char *str)
{
	while (*str)
	{
		char c = *(str++);
		addCharToBuffer(c);
	}
}
void setColorFore(Color colorForeground)
{

	currentForeColor.R = colorForeground.R;
	currentForeColor.G = colorForeground.G;
	currentForeColor.B = colorForeground.B;
	updateColor();
}
void setColorBack(Color colorBackground)
{
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
void rendererFlush()
{

	// fflush(stdout);
	// printf(buffer);
	terminalSetCursorPos((Vector2Int){0, 0});
	terminalDrawText(buffer);
	// printf("\033[0m"); // reset ansi
	// fflush(stdout);
	nrOfElements = 0;
	updateColor();
}
