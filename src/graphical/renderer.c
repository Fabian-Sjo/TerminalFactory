#include <stdlib.h>
#include <stdio.h>

#include "sprite.h"
#include "color.h"
#include "renderer.h"
#include "canvas.h"


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

void addCanvasToBuffer(Canvas *canvas)
{
	for (int y = 0; y < canvasGetSize(canvas).y; y++)
	{
		for (int x = 0; x < canvasGetSize(canvas).x; x++)
		{
			addSpriteToBuffer(canvasGetSprite(canvas, (Vector2Int){x, y}));
		}
		newLine();
	}

}
void addSpriteToBuffer(Sprite sprite)
{
	if (
		currentForeColor.R != (sprite.colorFore.R) ||
		currentForeColor.G != (sprite.colorFore.G) ||
		currentForeColor.B != (sprite.colorFore.B))
	{
		setColorFore((sprite.colorFore));
	}
	if (
		currentBackColor.R != (sprite.colorBack.R) ||
		currentBackColor.G != (sprite.colorBack.G) ||
		currentBackColor.B != (sprite.colorBack.B))
	{
		setColorBack((sprite.colorBack));
	}
	char c = sprite.icon;
	if (c == '\0')
		c = ' ';
	addCharToBuffer(c);
}
void addSpriteIdToBuffer(SpriteID spriteId)
{
	Sprite *sprite = getSprite(spriteId);
	addSpriteToBuffer(*sprite);
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
void flush()
{
	fflush(stdout);
	printf(buffer);
	fflush(stdout);
	nrOfElements = 0;
}
