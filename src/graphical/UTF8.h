#ifndef UTF8_H
#define UTF8_H
#include <stdint.h>
typedef struct UTF8
{
	char data[4];
} UTF8;
int UTFByteLength(UTF8);
#endif
