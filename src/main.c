
#include "terminal/terminal.h"

#include <stdio.h>

void keyListen(KeyEvent event){

};

int main()
{
	terminalInit();
	terminalSetKeyEventListener(&keyListen);
	while (1)
	{
		poolInput();
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
