#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

#define COLOR_BLACK (Color){0, 0, 0}
#define COLOR_RED (Color){255, 0, 0}
#define COLOR_WHITE (Color){255, 255, 255}
#define COLOR_TRANSPARENT (Color){-1, -1, -1}

#define COLOR_BLACK_CONST {0, 0, 0}
#define COLOR_RED_CONST {255, 0, 0}
#define COLOR_WHITE_CONST {255, 255, 255}
#define COLOR_TRANSPARENT_CONST {-1, -1, -1}

typedef struct
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
} Color;
struct HexColor
{
	char hex[7];
};

// Compares two colors and returns a non-zero value when they are equal.
int colorEquals(Color a, Color b);
// Converts a color to a hex-string representation suitable for display or debugging.
struct HexColor colorAsHex(Color color);
#endif
