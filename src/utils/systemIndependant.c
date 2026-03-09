#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

void msSleep(int ms)
{
#ifdef _WIN32
	Sleep(ms);
#else
	usleep(ms * 1000);
#endif
}