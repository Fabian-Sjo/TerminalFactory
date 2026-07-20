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
	int progress;
	int length;
};

// Computes a path from start to end through the provided world and returns a result object. The search is limited to a fixed radius and may return a partial path if the destination is unreachable.
PathFinderPath getPath(Vector2Int start, Vector2Int end, World *world);


#endif