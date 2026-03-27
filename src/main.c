
#include "terminal/terminal.h"
#include "utils/systemIndependant.h"
#include <windows.h>
#include <stdio.h>

int main()
{

	terminalInit();
	while (1)
	{
		poolInput();
		

		// printf("isPressed %d   ",terminalIsKeyPressed(KEY_A));
		// printf("isHeld %d   ",terminalIsKeyHeld(KEY_A));
		// printf("isReleased %d\n",terminalIsKeyReleased(KEY_A));

		msSleep(10);
	};

	return 0;
}
/*
	  > - > - > - > - > - > - > - > - > - > - > - > - > - > - > - > - > - > - > - > -
A			  v
A			  v
A			  v
< < < < < < < <

*/
