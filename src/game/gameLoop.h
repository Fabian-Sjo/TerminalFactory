#ifndef GAMELOOP_H
#define GAMELOOP_H

void addFunctionStart(void (*function)());
void addFunctionLoop(void (*function)(double deltaTime));
void addFunctionStop(void (*function)());
void setFps(int fps);
void startGame();
void stopGame();

#endif
