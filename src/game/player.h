#ifndef PLAYER_H
#define PLAYER_H
#include "../utils/vector2.h"

typedef struct Player
{
	Vector2Int position;
} Player;
void tick(long deltaTime);

Player *playerNew();

#endif