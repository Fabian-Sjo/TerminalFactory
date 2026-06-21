#ifndef PATHFINDER_H
#define PATHFINDER_H
#include "world.h"

typedef struct Path PathFinderPath;
enum PathFinderResult
{
	PATHFINDER_SUCCESS = 0,
	PATHFINDER_PARTIAL_PATH = 1,
	PATHFINDER_ERROR = -1
} PathFinderResult;
struct Path
{
	enum PathFinderResult result;
	Vector2Int *points;
	int length;
};

PathFinderPath getPath(Vector2Int start, Vector2Int end, World *world);


#endif