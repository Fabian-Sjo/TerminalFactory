#include "vector2.h"

Vector2Int vecSubI(Vector2Int vecA, Vector2Int vecB)
{
	vecA.x -= vecB.x;
	vecA.y -= vecB.y;
	return vecA;
}
Vector2Int vecAddI(Vector2Int vecA, Vector2Int vecB)
{
	vecA.x += vecB.x;
	vecA.y += vecB.y;
	return vecA;
}
Vector2Int vecDivI(Vector2Int vecA, Vector2Int vecB)
{
	vecA.x /= vecB.x;
	vecA.y /= vecB.y;
	return vecA;
}