#include "../utils/vector2.h"
#include "player.h"
#include <stdlib.h>

Player *playerNew()
{
	Player *player = malloc(sizeof(Player));
	player->position = (Vector2Float){0, 0};
	player->speed = 15;
	return player;
}