#include "groundTile.h"
#include <stdlib.h>


Sprite getGroundTileSprite(GroundTile *groundTile)
{
	if (groundTile == NULL)
	{
		Sprite sprite = {" ", COLOR_BLACK, COLOR_BLACK};
		return sprite;
	}
	return (groundTile->sprite);
}
