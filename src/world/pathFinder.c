#include "pathFinder.h"
#include "world.h"
#include "../utils/ADT.h"


Path getPath(Vector2Int start, Vector2Int end, World *world)
{
	int PATH_MAX_RADIUS = 20;
	int isFound = false;

	DynamicArray *nonCheckedPositions = dynamicArrayCreate(sizeof(Vector2Int), 32);
	dynamicArrayAdd(nonCheckedPositions,&start);


	LinkedList *takenPath = dynamicArrayCreate(sizeof(Vector2Int), 32);	 
	int checkedPositionsCount = 0;

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