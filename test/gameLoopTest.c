#include "../src/game/gameLoop.h"
#include <stdio.h>
void funcA(double delta)
{
	static long ticks = 0;
	ticks++;
	printf("A delta: %f\n", delta);
}
void funcB(double delta)
{
	static long ticks = 0;
	ticks++;
	printf("--delta: %f\n", delta);
}
void funcC(double delta)
{
	static long ticks = 0;
	ticks++;
	printf("C delta: %f\n", delta);
}
int main(int argc, char const *argv[])
{
	gameLoopAddFunctionLoop(&funcA, 1);
	gameLoopAddFunctionLoop(&funcB, 3);
	//gameLoopAddFunctionLoop(&funcC, 3);
	gameLoopStartGame();
	return 0;
}
