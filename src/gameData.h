#ifndef GAMEDATA_H
#define GAMEDATA_H

#include "game/player.h"
//#include "game/input.h"
#include "world/world.h"
#include "graphical/canvas.h"

typedef struct GameData
{
	//KeyEvent keyevent

	long frame;
	long tick;
	World *activeWorld;
} GameData;

#endif