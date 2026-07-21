#define true 1
#define false 0
#include <time.h>
#include "../utils/systemIndependant.h"
#define NS_PER_SEC 1000000000L
#define NS_PER_MS 1000000L
#define MS_PER_S 1000L
#include <stdio.h>
double updateDelta = 10;

int isRunning = false;
struct timespec lastFrame;
struct LoopingFunction
{
	void (*function)(double deltaTime);
	double targetDelta;
	struct timespec lastActivation;
};
int nrOfFunctionsStart = 0;
void (*functionsStart[5])(); // TODO hardcoded 5

int nrOfFunctionsLoop = 0;
struct LoopingFunction functionsLoop[5];

int nrOfFunctionsStop = 0;
void (*functionsStop[5])(); // TODO hardcoded 5

void updateUpdateRate()
{
	double minDelta = -1;
	for (size_t i = 0; i < nrOfFunctionsLoop; i++)
	{
		struct LoopingFunction *thisFunc = &functionsLoop[i];
		if ((thisFunc->targetDelta * 0.8) < minDelta || minDelta == -1)
			minDelta = thisFunc->targetDelta * 0.5;
	}
	if (minDelta == -1)
		isRunning = false;
	else
		updateDelta = minDelta;
}

void gameLoopAddFunctionStart(void (*function)())
{
	functionsStart[nrOfFunctionsStart++] = function;
}
void gameLoopAddFunctionLoop(void (*function)(double deltaTime), int fps)
{
	double delta = 1.0 / fps;
	functionsLoop[nrOfFunctionsLoop++] = (struct LoopingFunction){
		.function = function,
		.lastActivation = {0},
		.targetDelta = delta};
	updateUpdateRate();
}
void gameLoopAddFunctionStop(void (*function)())
{
	functionsStop[nrOfFunctionsStop++] = function;
}
long timeDiff(struct timespec earlierTime, struct timespec laterTime)
{
	return (laterTime.tv_sec - earlierTime.tv_sec) * NS_PER_SEC +
		   (laterTime.tv_nsec - earlierTime.tv_nsec);
}
void gameLoop()
{
	struct timespec frameStart, frameEnd, funcStart;

	long ticks = 0;
	while (isRunning)
	{
		long targetFrameNs = updateDelta * NS_PER_SEC;
		ticks++;
		timespec_get(&frameStart, TIME_UTC);

		long deltaTime = timeDiff(lastFrame, frameStart);

		int lowestSleepIndex = 0;
		for (int i = 0; i < nrOfFunctionsLoop; i++)
		{
			struct LoopingFunction *thisFunc = &functionsLoop[i];

			timespec_get(&funcStart, TIME_UTC);
			long timeSinceLastTrigger = timeDiff(thisFunc->lastActivation, funcStart);
			if (timeSinceLastTrigger >= thisFunc->targetDelta * NS_PER_SEC)
			{
				thisFunc->function(((double)timeSinceLastTrigger) / NS_PER_SEC);
				thisFunc->lastActivation = frameStart;
				timespec_get(&thisFunc->lastActivation, TIME_UTC);
			}
		}

		timespec_get(&frameEnd, TIME_UTC);

		long frameTime =
			(frameEnd.tv_sec - frameStart.tv_sec) * NS_PER_SEC +
			(frameEnd.tv_nsec - frameStart.tv_nsec);

		long sleepNs = targetFrameNs - frameTime;
		lastFrame = frameEnd;
		if (sleepNs > 0)
		{
			msSleep(sleepNs / NS_PER_MS);
		}
	}
}
void gameLoopStartGame()
{
	for (int i = 0; i < nrOfFunctionsStart; i++)
	{
		functionsStart[i]();
	}
	isRunning = true;
	gameLoop();
}
void gameLoopStopGame()
{
	for (int i = 0; i < nrOfFunctionsStop; i++)
	{
		functionsStop[i]();
	}
	isRunning = false;
}