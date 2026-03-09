#include "game/input.h"
#include <conio.h>

int neededChecksWithoutInput = 2;

char *keyToString(Key key)
{
	switch (key)
	{

	case KEY_NONE:
		return "none";
	case KEY_ESC:
		return "esc";
	case KEY_A:
		return "a";
	case KEY_B:
		return "b";
	case KEY_C:
		return "c";
	case KEY_D:
		return "d";
	case KEY_E:
		return "e";
	case KEY_F:
		return "f";
	case KEY_G:
		return "g";
	case KEY_H:
		return "h";
	case KEY_I:
		return "i";
	case KEY_J:
		return "j";
	case KEY_K:
		return "k";
	case KEY_L:
		return "l";
	case KEY_M:
		return "m";
	case KEY_N:
		return "n";
	case KEY_O:
		return "o";
	case KEY_P:
		return "p";
	case KEY_Q:
		return "q";
	case KEY_R:
		return "r";
	case KEY_S:
		return "s";
	case KEY_T:
		return "t";
	case KEY_U:
		return "u";
	case KEY_V:
		return "v";
	case KEY_W:
		return "w";
	case KEY_X:
		return "x";
	case KEY_Y:
		return "y";
	case KEY_Z:
		return "z";
	case KEY_SPACE:
		return "space";
	case KEY_LEFT_ARROW:
		return "left";
	case KEY_RIGHT_ARROW:
		return "right";
	case KEY_UP_ARROW:
		return "up";
	case KEY_DOWN_ARROW:
		return "down";
	default:
		return KEY_NONE;
	}
}
Key getNormalKey(int c)
{
	switch (c)
	{
	case 27:
		return KEY_ESC;
	case 'a':
		return KEY_A;
	case 'b':
		return KEY_B;
	case 'c':
		return KEY_C;
	case 'd':
		return KEY_D;
	case 'e':
		return KEY_E;
	case 'f':
		return KEY_F;
	case 'g':
		return KEY_G;
	case 'h':
		return KEY_H;
	case 'i':
		return KEY_I;
	case 'j':
		return KEY_J;
	case 'k':
		return KEY_K;
	case 'l':
		return KEY_L;
	case 'm':
		return KEY_M;
	case 'n':
		return KEY_N;
	case 'o':
		return KEY_O;
	case 'p':
		return KEY_P;
	case 'q':
		return KEY_Q;
	case 'r':
		return KEY_R;
	case 's':
		return KEY_S;
	case 't':
		return KEY_T;
	case 'u':
		return KEY_U;
	case 'v':
		return KEY_V;
	case 'w':
		return KEY_W;
	case 'x':
		return KEY_X;
	case 'y':
		return KEY_Y;
	case 'z':
		return KEY_Z;
	case ' ':
		return KEY_SPACE;
	default:
		return KEY_INVALID;
	}
}
Key getFunctionKey(int c)
{
	switch (c)
	{
	case 72: // up arrow
		return KEY_UP_ARROW;
	case 75: // left arrow
		return KEY_LEFT_ARROW;
	case 77: // right arrow
		return KEY_RIGHT_ARROW;
	case 80: // down arrow
		return KEY_DOWN_ARROW;
	default:
		return KEY_INVALID;
	}
}
Key getKey()
{
	if (_kbhit())
	{
		int key = _getch();
		int function = 0;
		if (_kbhit())
			function = _getch();
			
		Key thisKey = KEY_NONE;
 		if (key != 224)
			thisKey = getNormalKey(key);
		else
			thisKey = getFunctionKey(function);

		return thisKey;
	}
	return KEY_NONE;
}
KeyEvent getKeyEvent()
{
	KeyEvent keyEvent = {KEY_NONE, KEY_NONE, KEY_NONE};
	static Key heldKey = KEY_NONE;
	static int checksWithoutInput = 0;

	Key thisKey = getKey();

	if (thisKey == KEY_NONE)
	{
		if (checksWithoutInput++ == neededChecksWithoutInput)
		{
			checksWithoutInput = 0;
			keyEvent.released = heldKey;
			heldKey = KEY_NONE;
		}else
		{
			keyEvent.held = heldKey;
		}
		
	}
	else if(heldKey != thisKey)
	{
		checksWithoutInput = 0;
		keyEvent.released = heldKey;
		heldKey = thisKey;
		keyEvent.pressed = thisKey;
	}else
	{
		checksWithoutInput = 0;
		heldKey = thisKey;
		keyEvent.held = thisKey;
	}
	

	return keyEvent;
}