#ifndef GAMELOOP_H
#define GAMELOOP_H

// Registers a callback to run once when the game starts.
void gameLoopAddFunctionStart(void (*function)());
// Registers a callback to run every frame with the elapsed delta time.
void gameLoopAddFunctionLoop(void (*function)(double deltaTime),int targetFps);
// Registers a callback to run once when the game stops.
void gameLoopAddFunctionStop(void (*function)());

// Starts the game loop.
void gameLoopStartGame();
// Stops the game loop.
void gameLoopStopGame();

#endif
