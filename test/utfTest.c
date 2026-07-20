#include <stdio.h>
#include "../src/graphical/renderer.h"
#include "../src/graphical/canvas.h"
#include "../src/terminal/terminal.h"
#include <wchar.h>
#include <locale.h>
void printBits(size_t size, size_t amount, const void *ptr)
{
	const unsigned char *b = ptr;

	for (size_t nr = 0; nr < amount; nr++)
	{
		for (int i = (int)size - 1; i >= 0; i--)
		{
			unsigned char byte = b[nr * size + i];

			for (int j = 7; j >= 0; j--)
				putchar(((byte >> j) & 1) + '0');
		}

		putchar(' ');
	}
}

void test(char *data)
{
	UTF8 c;
	for (size_t i = 0; i < 4; i++)
	{
		c.data[i] = data[i];
	}

	printf("%.*s = ", 4, c.data);
	printBits(sizeof(char), 4, c.data);
	printf("length = %1d", UTFByteLength(c));
	putchar('\n');
}

int main(int argc, char const *argv[])
{
	test("");
	test("A");
	test("Ä");
	test("ࠁ");
	test("𓉤");
	printf("{╾╋ ║ ═ ─ │ ┼ █}\n");
	Canvas *canvas = canvasNew((Vector2Int){3, 3});
	canvasFill(canvas, (Sprite){.icon = "/"});
	canvasSetSprite(canvas, (Vector2Int){1, 1}, (Sprite){.icon = "☁"});
	rendererDrawCanvas(canvas);

	rendererFlush();
	return 0;
}
