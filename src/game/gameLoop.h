void addFunctionStart(void (*function)());
void addFunctionLoop(void (*function)(long deltaTime));
void addFunctionStop(void (*function)());
setFps(int fps);
void startGame();
void stopGame();