#include "UTF8.h"
int UTFByteLength(UTF8 utf)
{
	unsigned char b = utf.data[0];

	if ((b & 0x80) == 0x00)
		return 1; // 0xxxxxxx
	if ((b & 0xE0) == 0xC0)
		return 2; // 110xxxxx
	if ((b & 0xF0) == 0xE0)
		return 3; // 1110xxxx
	if ((b & 0xF8) == 0xF0)
		return 4; // 11110xxx

	return 0; // Invalid UTF-8 leading byte

	0b00000000; // 1 byte
	0b11000000; // 2 byte
	0b11100000; // 3 byte
	0b11110000; // 4 byte

	0b10000000;
}