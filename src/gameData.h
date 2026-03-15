#ifndef GAMEDATA_H
#define GAMEDATA_H

#include "game/player.h"
#include "game/input.h"
#include "world/world.h"
#include "graphical/canvas.h"

typedef struct GameData
{
	KeyEvent keyevent;
	Player *player;

	long frame;
	long tick;

	Vector2Int screenSize;

	World *activeWorld;
	Canvas *canvas;
} GameData;

#endif