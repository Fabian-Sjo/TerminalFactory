#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "pathFinder.h"
#include "../utils/ADT.h"

#define MAX_SEARCH_RADIUS 40
#define ROW (MAX_SEARCH_RADIUS * 2 + 1)
#define COL (MAX_SEARCH_RADIUS * 2 + 1)
#define ALLOW_CORNER_CUTTING false

typedef struct
{
	int parent_x;
	int parent_y;
	double f;
	double g;
	double h;
} Node;

typedef enum
{
	TRAVERSE_OPEN,
	TRAVERSE_BLOCKED,
} TRAVERSE_TYPE;

typedef struct
{
	int row;
	int col;
} OpenNode;

static Vector2Int worldToLocal(Vector2Int origin, Vector2Int world)
{
	return (Vector2Int){
		world.x - origin.x + MAX_SEARCH_RADIUS,
		world.y - origin.y + MAX_SEARCH_RADIUS};
}

static Vector2Int localToWorld(Vector2Int origin, Vector2Int local)
{
	return (Vector2Int){
		origin.x + local.x - MAX_SEARCH_RADIUS,
		origin.y + local.y - MAX_SEARCH_RADIUS};
}

static int isInBounds(Vector2Int pos)
{
	return pos.x >= 0 &&
		   pos.x < ROW &&
		   pos.y >= 0 &&
		   pos.y < COL;
}

static int isUnBlocked(TRAVERSE_TYPE grid[][COL], Vector2Int pos)
{
	if (!isInBounds(pos))
		return 0;

	return grid[pos.x][pos.y] != TRAVERSE_BLOCKED;
}

static int inOpenList(OpenNode openList[],
					  int openSize,
					  Vector2Int pos)
{
	for (int i = 0; i < openSize; i++)
	{
		if (openList[i].row == pos.x &&
			openList[i].col == pos.y)
		{
			return 1;
		}
	}

	return 0;
}

static int isDestination(Vector2Int src,
						 Vector2Int dest)
{
	return src.x == dest.x &&
		   src.y == dest.y;
}

static double calculateHValue(Vector2Int src, Vector2Int dest)
{
	double dx = src.x - dest.x;
	double dy = src.y - dest.y;

	return sqrt(dx * dx + dy * dy);
}

static PathFinderPath tracePath(Node nodeDetails[][COL], enum PathFinderResult result,
								Vector2Int dest)
{

	Vector2Int pathPoints[ROW * COL];

	int count = 0;

	int row = dest.x;
	int col = dest.y;

	if (!isInBounds((Vector2Int){row, col}))
	{

		return (PathFinderPath){
			.length = -1,
			.points = NULL,
			.result = PATHFINDER_ERROR};
	}

	while (!(nodeDetails[row][col].parent_x == row &&
			 nodeDetails[row][col].parent_y == col))
	{
		if (count >= ROW * COL)
		{
			printf("TRACE PATH OVERFLOW\n");

			return (PathFinderPath){
				.length = -1,
				.points = NULL,
				.result = PATHFINDER_ERROR};
		}

		pathPoints[count].x = row;
		pathPoints[count].y = col;
		count++;

		int parentRow =
			nodeDetails[row][col].parent_x;

		int parentCol =
			nodeDetails[row][col].parent_y;

		if (!isInBounds((Vector2Int){
				parentRow,
				parentCol}))
		{
			printf("INVALID PARENT NODE\n");
			return (PathFinderPath){
				.length = -1,
				.points = NULL,
				.result = PATHFINDER_ERROR};
		}

		row = parentRow;
		col = parentCol;
	}

	if (count >= ROW * COL)
	{
		free(pathPoints);

		return (PathFinderPath){
			.length = -1,
			.points = NULL,
			.result = PATHFINDER_ERROR};
	}

	pathPoints[count].x = row;
	pathPoints[count].y = col;
	count++;

	Vector2Int *revesedPoints = malloc(count * sizeof(Vector2Int));
	if (revesedPoints == NULL)
	{
		return (PathFinderPath){.length = -1, .points = NULL, .result = PATHFINDER_ERROR};
	}
	for (size_t i = 0; i < count; i++)
	{
		revesedPoints[i] = pathPoints[count - i - 1];
	}

	return (PathFinderPath){
		.length = count,
		.points = revesedPoints,
		.result = result};
}

PathFinderPath aStarSearch(int grid[][COL],
						   Vector2Int src,
						   Vector2Int dest)
{
	if (!isInBounds(src)
		//||!isInBounds(dest)
	)
	{
		return (PathFinderPath){
			.length = -1,
			.points = NULL,
			.result = PATHFINDER_ERROR};
	}

	if (!isUnBlocked(grid, src)
		//	||!isUnBlocked(grid, dest)
	)
	{
		return (PathFinderPath){
			.length = -1,
			.points = NULL,
			.result = PATHFINDER_ERROR};
	}

	if (isDestination(src, dest))
	{
		return (PathFinderPath){
			.length = 0,
			.points = NULL,
			.result = PATHFINDER_SUCCESS};
	}

	int closedList[ROW][COL] = {0};
	Node nodeDetails[ROW][COL];

	for (int r = 0; r < ROW; r++)
	{
		for (int c = 0; c < COL; c++)
		{
			nodeDetails[r][c].f = DBL_MAX;
			nodeDetails[r][c].g = DBL_MAX;
			nodeDetails[r][c].h = DBL_MAX;
			nodeDetails[r][c].parent_x = -1;
			nodeDetails[r][c].parent_y = -1;
		}
	}

	nodeDetails[src.x][src.y].f = 0;
	nodeDetails[src.x][src.y].g = 0;
	nodeDetails[src.x][src.y].h = 0;
	nodeDetails[src.x][src.y].parent_x = src.x;
	nodeDetails[src.x][src.y].parent_y = src.y;

	OpenNode openList[ROW * COL];
	int openSize = 0;

	openList[openSize++] =
		(OpenNode){src.x, src.y};

	int rowOffset[4] =
		{-1, 0, 1, 0};

	int colOffset[4] =
		{0, -1, 0, 1};

	Vector2Int closestNode = src;
	double closestH = calculateHValue(src, dest);

	while (openSize > 0)
	{
		double minF = DBL_MAX;
		int minIndex = -1;

		for (int i = 0; i < openSize; i++)
		{
			int r = openList[i].row;
			int c = openList[i].col;

			if (nodeDetails[r][c].f < minF)
			{
				minF = nodeDetails[r][c].f;
				minIndex = i;
			}
		}

		if (minIndex == -1)
		{
			return (PathFinderPath){
				.length = -1,
				.points = NULL,
				.result = PATHFINDER_ERROR};
		}

		OpenNode current =
			openList[minIndex];

		openList[minIndex] =
			openList[--openSize];

		int row = current.row;
		int col = current.col;

		closedList[row][col] = 1;

		for (int i = 0; i < 4; i++)
		{
			Vector2Int newPos =
				{
					row + rowOffset[i],
					col + colOffset[i]};

			if (!isInBounds(newPos))
				continue;

			if (isDestination(newPos, dest))
			{
				nodeDetails[newPos.x][newPos.y].parent_x = row;
				nodeDetails[newPos.x][newPos.y].parent_y = col;

				return tracePath(nodeDetails, PATHFINDER_SUCCESS, dest);
			}

			if (closedList[newPos.x][newPos.y])
				continue;

			if (!isUnBlocked(grid, newPos))
				continue;

			double gNew = nodeDetails[row][col].g + 1.0;

			double hNew = calculateHValue(newPos, dest);
			if (hNew < closestH)
			{
				closestH = hNew;
				closestNode = newPos;
			}
			double fNew = gNew + hNew;

			if (fNew < nodeDetails[newPos.x][newPos.y].f)
			{
				nodeDetails[newPos.x][newPos.y].f = fNew;
				nodeDetails[newPos.x][newPos.y].g = gNew;
				nodeDetails[newPos.x][newPos.y].h = hNew;

				nodeDetails[newPos.x][newPos.y].parent_x = row;
				nodeDetails[newPos.x][newPos.y].parent_y = col;

				if (!inOpenList(openList,
								openSize,
								newPos))
				{
					if (openSize >= ROW * COL)
					{
						printf("OPEN LIST OVERFLOW\n");

						return (PathFinderPath){
							.length = -1,
							.points = NULL,
							.result = PATHFINDER_ERROR};
					}

					openList[openSize++] =
						(OpenNode){newPos.x, newPos.y};
				}
			}
		}
	}

	return tracePath(nodeDetails, PATHFINDER_PARTIAL_PATH, closestNode);
}

PathFinderPath getPath(Vector2Int start,
					   Vector2Int end,
					   World *world)
{
	TRAVERSE_TYPE navGrid[ROW][COL];

	for (int row = 0; row < ROW; row++)
	{
		for (int col = 0; col < COL; col++)
		{
			Tile *tile =
				getTile(world,
						start.x + row - MAX_SEARCH_RADIUS,
						start.y + col - MAX_SEARCH_RADIUS);

			navGrid[row][col] = TRAVERSE_OPEN;

			if (!isTileWalkable(world, (Vector2Int){start.x + row - MAX_SEARCH_RADIUS,
												   start.y + col - MAX_SEARCH_RADIUS}))
			{
				navGrid[row][col] = TRAVERSE_BLOCKED;
			}
		}
	}

	Vector2Int localStart =
		{
			MAX_SEARCH_RADIUS,
			MAX_SEARCH_RADIUS};

	Vector2Int localEnd =
		worldToLocal(start, end);

	PathFinderPath path =
		aStarSearch(navGrid,
					localStart,
					localEnd);

	if (path.result != PATHFINDER_ERROR && path.length > 0)
	{
		for (int i = 0; i < path.length; i++)
		{
			path.points[i] =
				localToWorld(start,
							 path.points[i]);
		}
	}

	return path;
}