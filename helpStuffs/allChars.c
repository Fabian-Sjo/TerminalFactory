#include <stdio.h>

int main(int argc, char const *argv[])
{
	char *cha = "┘";

	printf("char: %s %d\n", cha, cha);
	char c = '!' - 1;
	while (c++ != 0)
	{
		printf("\033[38;2;255;255;255m%c ", c);
		if (c % 16 == 0)
			printf("\n");
	}

	return 0;
}
