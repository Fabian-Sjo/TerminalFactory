#include "../src/utils/vector2.h"
#include "../src/utils/systemIndependant.h"
#include "../src/graphical/canvas.h"
#include "../src/graphical/sprite.h"
#include <stdlib.h>

#define true 1
#define false 0

Vector2Int screenSize = {10, 10};
Vector2Int screenSafezone = {0, 10};
Canvas *canvas;

char testText[] = "TestText";

typedef struct Frame
{
    Vector2Int position;
    Vector2Int size;
    struct Frame *parent;
} Frame;

typedef struct
{
	Vector2Int position;
	Frame *parent;
	int isSelected;
	int textLength;
	char *text;
	void (*onClick)(int startClick);
} Button;

void redrawCanvasAndGui()
{
	printf("\033[2J\033[H");
	NineRect nineRect = {
		{{(Sprite){'\\'}, (Sprite){'|'}, (Sprite){'/'}},
		 {(Sprite){'='}, (Sprite){'.'}, (Sprite){'='}},
		 {(Sprite){'/'}, (Sprite){'|'}, (Sprite){'\\'}}}};
	for (int i = 0; i < 1; i++)
	{
		canvasDrawNineRect(canvas, (Vector2Int){i, i}, (Vector2Int){screenSize.x - i * 2, screenSize.y - i * 2}, nineRect, FILL_NONE);
	}
}
renderFrame(Frame *frame)
{
	NineRect nineRect = {
		{{(Sprite){'\\'}, (Sprite){'|'}, (Sprite){'/'}},
		 {(Sprite){'='}, (Sprite){'.'}, (Sprite){'='}},
		 {(Sprite){'/'}, (Sprite){'|'}, (Sprite){'\\'}}}};

	Vector2Int offset = (Vector2Int){0, 0};
	if (frame->parent != NULL)
		offset = frame->parent->position;
	canvasDrawNineRect(canvas, vecAddI(frame->position, offset), frame->size, nineRect, FILL_NONE);
}
renderButton(Button *button)
{
	Vector2Int offset = (Vector2Int){0, 0};
	if (button->parent != NULL)
		offset = button->parent->position;
	for (size_t i = 0; i < button->textLength; i++)
	{
		Sprite sprite = (Sprite){.icon = button->text[i]};
		canvasSetSprite(canvas, vecAddI(vecAddI(button->position, offset), (Vector2Int){i, 0}), sprite);
	}
}
void render()
{
	terminalSetCursorPos((Vector2Int){0, 0});
	rendererDrawCanvas(canvas);
	rendererFlush();
}
int main(int argc, char const *argv[])
{
	Frame *testFrame = malloc(sizeof(Frame));
	testFrame->position = (Vector2Int){10, 10};
	testFrame->size = (Vector2Int){10, 10};

	Frame *testFrame2 = malloc(sizeof(Frame));
	testFrame2->position = (Vector2Int){5, 5};
	testFrame2->size = (Vector2Int){20, 20};

	testFrame->parent = testFrame2;

	Button *testButton = malloc(sizeof(Button));
	testButton->parent = testFrame;
	testButton->position = (Vector2Int){1, 1};
	testButton->isSelected = false;
	testButton->textLength = 4;
	testButton->text = &testText;

	terminalInit();
	Vector2Int termsize = getTermSize();
	screenSize = vecDivI(vecSubI(termsize, screenSafezone), (Vector2Int){1 + 1, 1});
	canvas = canvasNew(screenSize);
	redrawCanvasAndGui();
	renderFrame(testFrame);
	renderButton(testButton);
	render();
	return 0;
}
