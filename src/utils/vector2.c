#include "vector2.h"
#include <stdlib.h>

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
Vector2Int vecMulI(Vector2Int vecA, Vector2Int vecB)
{
	vecA.x *= vecB.x;
	vecA.y *= vecB.y;
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

Vector2Int vecFloor(Vector2Float vec)
{

	return (Vector2Int){
		vec.x < 0 ? (int)(vec.x + 1) : (int)(vec.x),
		vec.y < 0 ? (int)(vec.y + 1) : (int)(vec.y)};
}
bool isPointInRect(Vector2Int point, Vector2Int cornerA, Vector2Int cornerB)
{
	Vector2Int upperLeft = {min(cornerA.x, cornerB.x), min(cornerA.y, cornerB.y)};
	Vector2Int bottomRight = {max(cornerA.x, cornerB.x), max(cornerA.y, cornerB.y)};
	return (point.x >= upperLeft.x &&
			point.y >= upperLeft.y &&
			point.x <= bottomRight.x &&
			point.y <= bottomRight.y);
}
Vector2Int rectSizeI(Rect2Int rect)
{
	return vecSubI(rect.end, rect.pos);
}