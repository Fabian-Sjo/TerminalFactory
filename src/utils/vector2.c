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
Vector2Int vecRound(Vector2Float vec)
{
	return (Vector2Int){
		(int)(vec.x + 0.5),
		(int)(vec.y + 0.5)};
}