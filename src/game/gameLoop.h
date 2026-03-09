void addFunctionStart(void (*function)());
void addFunctionLoop(void (*function)(long deltaTime));
void addFunctionStop(void (*function)());
void setFps(int fps);
void startGame();
void stopGame();