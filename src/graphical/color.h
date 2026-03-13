#ifndef COLOR_H
#define COLOR_H


#include <stdint.h>

#define COLOR_BLACK (Color){0, 0, 0}
#define COLOR_WHITE (Color){255, 255, 255}
#define COLOR_TRANSPARENT (Color){-1, -1, -1}

#define COLOR_BLACK_CONST {0, 0, 0}
#define COLOR_WHITE_CONST {255, 255, 255}
#define COLOR_TRANSPARENT_CONST {-1, -1, -1}

typedef struct
{
	int R;
	int G;
	int B;
} Color;
int colorEquals(Color a, Color b);

#endif
