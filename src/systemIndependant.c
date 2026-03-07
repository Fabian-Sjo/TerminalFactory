#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

void sleep(int ms)
{
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}