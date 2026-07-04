#include "../src/graphical/color.h"
#include "../src/utils/systemIndependant.h"
#include <stdlib.h>

int main(int argc, char const *argv[])
{
	Color color = {255, 0, 128};
	struct HexColor hex = colorAsHex(color);
	printf(hex.hex);
	for (int i = -100; i < 300; i++)
	{
		printf(":%4d\n", (unsigned char)i);
	}
	
	return 0;
}
