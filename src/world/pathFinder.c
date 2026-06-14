#include "pathFinder.h"
#include "world.h"

Path getPath(Vector2Int start, Vector2Int end, World *world)
{
	int isFound = false;

	Path path;
	path.result = PATHFINDER_ERROR;
	while (isFound == false)
	{
		getTile(world, start.x, start.y);
		// TODO implement path finding algorithm, probably A*
		isFound = true;
	}


	path.points = NULL;
	path.length = 0;
	return path;
}