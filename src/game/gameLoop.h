#ifndef GAMELOOP_H
#define GAMELOOP_H

// Registers a callback to run once when the game starts.
void addFunctionStart(void (*function)());
// Registers a callback to run every frame with the elapsed delta time.
void addFunctionLoop(void (*function)(double deltaTime));
// Registers a callback to run once when the game stops.
void addFunctionStop(void (*function)());
// Sets the target frame rate for the game loop.
void setFps(int fps);
// Starts the game loop.
void startGame();
// Stops the game loop.
void stopGame();

#endif
