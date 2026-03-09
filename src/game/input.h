#ifndef INPUT
#define INPUT


typedef enum Key
{
	KEY_NONE,
	KEY_INVALID,
	KEY_ESC,
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_LEFT_ARROW,
	KEY_RIGHT_ARROW,
	KEY_UP_ARROW,
	KEY_DOWN_ARROW,
	KEY_SPACE
} Key;
typedef struct KeyEvent
{
	Key pressed;
	Key held;
	Key released;
} KeyEvent;

char *keyToString(Key key);
Key getKey();
KeyEvent getKeyEvent();
#endif