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

// #define vectorMod(vecA, vecB) {vecA.x % vecB.x, vecA.y % vecB.y}

// #define vectorStepPowTwo(a, b) ({(a).x & ((b).x - 1), (a).y & ((b).y - 1)})
//  #define vectorStep(vecA, vecB) {vecA.x x & (CHUNK_SIZE - 1), vecA.y x & (CHUNK_SIZE - 1)}

Vector2Int vecSubI(Vector2Int vecA, Vector2Int vecB);
Vector2Int vecAddI(Vector2Int vecA, Vector2Int vecB);
Vector2Int vecDivI(Vector2Int vecA, Vector2Int vecB);

#endif