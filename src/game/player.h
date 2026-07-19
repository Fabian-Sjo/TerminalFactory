#ifndef PLAYER_H
#define PLAYER_H
#include "../utils/vector2.h"

typedef struct Player
{
	Vector2Float position;
	float speed;
} Player;


// Advances the player state by the elapsed time DeltaTime.
void tick(long deltaTime);

// Allocates and initializes a new player object.
Player *playerNew();

#endif
