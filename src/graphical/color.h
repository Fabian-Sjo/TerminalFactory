#ifndef COLOR_H
#define COLOR_H


#include <stdint.h>

#define COLOR_BLACK {0, 0, 0}
#define COLOR_WHITE {255, 255, 255}
typedef struct
{
	int16_t R;
	int16_t G;
	int16_t B;
} Color;


#endif
