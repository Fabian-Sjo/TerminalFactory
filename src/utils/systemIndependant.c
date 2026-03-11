#include "vector2.h"

#ifdef _WIN32

#include <windows.h>

#else

#include <time.h>
#include <errno.h>

#endif

void msSleep(int ms)
{
#ifdef _WIN32
	Sleep(ms);
#else
	struct timespec ts;
	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000L;

	while (nanosleep(&ts, &ts) && errno == EINTR)
		; // retry if interrupted
#endif
}
Vector2Int getTermSize()
{
#ifdef _WIN32
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	int columns, rows;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	
	Vector2Int vec = {columns, rows};
	return vec;
#else

#endif
}