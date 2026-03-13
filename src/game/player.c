#include "../utils/vector2.h"
#include "player.h"
#include <stdlib.h>

Player *playerNew()
{
	Player *player = malloc(sizeof(Player));
	player->position = (Vector2Int){0, 0};
	return player;
}