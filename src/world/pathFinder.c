#include "pathFinder.h"
#include "world.h"
#include "../utils/ADT.h"
typedef PathTile PathTile;
struct PathTile
{
	Vector2Int pos;
	Vector2Int cameFrom;
	int gCost, hCost, fCost;
};
inline Vector2Int indexToOffset(int offset)
{
	switch (offset)
	{
	case 1:
		return (Vector2Int){-1, -1};
	case 2:
		return (Vector2Int){0, -1};
	case 3:
		return (Vector2Int){1, -1};
	case 4:
		return (Vector2Int){-1, 0};
	case 5:
		return (Vector2Int){1, 0};
	case 6:
		return (Vector2Int){-1, 1};
	case 7:
		return (Vector2Int){0, 1};
	case 8:
		return (Vector2Int){1, 1};

	default:
		break;
	}
}
Path getPath(Vector2Int start, Vector2Int end, World *world)
{
	int PATH_MAX_RADIUS = 20;
	int isFound = false;

	DynamicArray *open = dynamicArrayCreate(sizeof(PathTile), 32);
	dynamicArrayAdd(open, &start);

	LinkedList *closed = linkedListCreate(sizeof(PathTile));

	linkedListinsertAtEnd(closed, &start);
	linkedListinsertAtEnd(closed, &end);

	int checkedPositionsCount = 0;

	Path path;
	path.result = PATHFINDER_ERROR;
	while (isFound == false)
	{
		PathTile current = dynamicArrayGet(open, 0);
		linkedListinsertAtEnd(closed, current);

		for (size_t neighbourIndex = 0; neighbourIndex < 8; neighbourIndex++)
		{
			Vector2Int neighbourPos = vecAddI(indexToOffset(neighbourIndex), current.pos);
			getTile(world, neighbourPos.x, neighbourPos.y);
			int cost = 1;
		}

		// TODO implement path finding algorithm, probably A*
		isFound = true;
	}

	path.points = linkedListToArray(closed);
	path.length = 2;
	return path;
}