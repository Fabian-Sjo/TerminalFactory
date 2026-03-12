#ifndef VECTOR2_H
#define VECTOR2_H

typedef struct Vector2Float
{
	float x, y;
} Vector2Float;
typedef struct Vector2Int
{
	int x, y;
} Vector2Int;

#define vectorSub(vecA, vecB) {vecA.x-vecB.x,vecA.y-vecB.y}

#define vectorAdd(vecA, vecB) {vecA.x+vecB.x,vecA.y+vecB.y}






#endif