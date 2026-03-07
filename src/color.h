#ifndef COLOR
#define COLOR
#include <stdint.h>

#define COLOR_BLACK {0, 0, 0}
typedef struct
{
	int16_t R;
	int16_t G;
	int16_t B;
} Color;
#endif