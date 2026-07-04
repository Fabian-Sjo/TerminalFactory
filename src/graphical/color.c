#include "color.h"
int colorEquals(Color a, Color b)
{
	return (a.R == b.R && a.G == b.G && a.B == b.B);
}
struct HexColor colorAsHex(Color color)
{
	struct HexColor hexColor;
	hexColor.hex[0] = '#';
	unsigned char red = color.R;
	hexColor.hex[2] = (red & 0xF) + '0';
	hexColor.hex[1] = ((red >> 4) & 0xF) + '0';
	unsigned char green = color.G;
	hexColor.hex[4] = (green & 0xF) + '0';
	hexColor.hex[3] = ((green >> 4) & 0xF) + '0';
	unsigned char blue = color.B;
	hexColor.hex[6] = (blue & 0xF) + '0';
	hexColor.hex[5] = ((blue >> 4) & 0xF) + '0';
	for (size_t i = 1; i < 7; i++)
	{
		if (hexColor.hex[i] > '9')
			hexColor.hex[i] += 7;
	}

	return hexColor;
}