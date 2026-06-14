#ifndef PATHFINDER_H
#define PATHFINDER_H
#include "world.h"

typedef struct Path Path;
enum PathFinderResult
{
	PATHFINDER_SUCCESS,
	PATHFINDER_PARTIAL_PATH,
	PATHFINDER_ERROR
} PathFinderResult;
struct Path
{
	enum PathFinderResult result;
	Vector2Int *points;
	int length;
};

Path getPath(Vector2Int start, Vector2Int end, World *world);


#endif