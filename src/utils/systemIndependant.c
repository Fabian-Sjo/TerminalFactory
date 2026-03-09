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