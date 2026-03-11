#include "groundTile.h"
struct GroundTile
{
	Sprite sprite;
};

GroundTile GROUND_TILE_WATER =
	{
		.sprite = {'~',
				   COLOR_BLACK,
				   {0, 0, 100}}};
GroundTile GROUND_TILE_GRASS_1 =
	{
		.sprite = {'.',
				   COLOR_BLACK,
				   {0, 55, 0}}};
GroundTile GROUND_TILE_GRASS_2 =
	{
		.sprite = {':',
				   COLOR_BLACK,
				   {0, 67, 0}}};
GroundTile GROUND_TILE_GRASS_3 =
	{
		.sprite = {'^',
				   COLOR_BLACK,
				   {0, 80, 0}}};
Sprite getGroundTileSprite(GroundTile *groundTile)
{
	if (groundTile == NULL)
	{
		Sprite sprite = {' ', COLOR_BLACK, COLOR_BLACK};
		return sprite;
	}
	return (groundTile->sprite);
}
