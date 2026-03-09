#define true 1
#define false 0
#include <time.h>
#include "utils/systemIndependant.h"
#define NS_PER_SEC 1000000000L
#define NS_PER_MS 1000000L

int targerFps = 10;

int isRunning = false;
struct timespec lastTime;

int nrOfFunctionsStart = 0;
void (*functionsStart[5])(); // TODO hardcoded 5

int nrOfFunctionsLoop = 0;
void (*functionsLoop[5])(long deltaTime); // TODO hardcoded 5

int nrOfFunctionsStop = 0;
void (*functionsStop[5])(); // TODO hardcoded 5

setFps(int fps)
{
	targerFps = fps;
}

void addFunctionStart(void (*function)())
{
	functionsStart[nrOfFunctionsStart++] = function;
}
void addFunctionLoop(void (*function)(long deltaTime))
{
	functionsLoop[nrOfFunctionsLoop++] = function;
}
void addFunctionStop(void (*function)())
{
	functionsStop[nrOfFunctionsStop++] = function;
}
void gameLoop()
{
    struct timespec frameStart, now;

    long targetFrameNs = NS_PER_SEC / targerFps;

    while (isRunning)
    {
        timespec_get(&frameStart, TIME_UTC);

        long nsDiff =
            (frameStart.tv_sec - lastTime.tv_sec) * NS_PER_SEC +
            (frameStart.tv_nsec - lastTime.tv_nsec);

        lastTime = frameStart;

        for (int i = 0; i < nrOfFunctionsLoop; i++)
        {
            functionsLoop[i](nsDiff);
        }

        timespec_get(&now, TIME_UTC);

        long frameTime =
            (now.tv_sec - frameStart.tv_sec) * NS_PER_SEC +
            (now.tv_nsec - frameStart.tv_nsec);

        long sleepNs = targetFrameNs - frameTime;

        if (sleepNs > 0)
        {
            msSleep(sleepNs / NS_PER_MS);
        }
    }
}
void startGame()
{
	for (int i = 0; i < nrOfFunctionsStart; i++)
	{
		functionsStart[i]();
	}
	isRunning = true;
	gameLoop();
}
void stopGame()
{
	for (int i = 0; i < nrOfFunctionsStop; i++)
	{
		functionsStop[i]();
	}
	isRunning = false;
}