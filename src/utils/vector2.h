#ifndef VECTOR2_H
#define VECTOR2_H
#include <stdbool.h>

typedef struct Vector2Float
{
	float x, y;
} Vector2Float;
typedef struct Vector2Int
{
	int x, y;
} Vector2Int;
typedef struct Rect2Int
{
	Vector2Int pos, end;
} Rect2Int;

// #define vectorMod(vecA, vecB) {vecA.x % vecB.x, vecA.y % vecB.y}

// #define vectorStepPowTwo(a, b) ({(a).x & ((b).x - 1), (a).y & ((b).y - 1)})
//  #define vectorStep(vecA, vecB) {vecA.x x & (CHUNK_SIZE - 1), vecA.y x & (CHUNK_SIZE - 1)}

// Returns the component-wise subtraction of two integer vectors.
Vector2Int vecSubI(Vector2Int vecA, Vector2Int vecB);
// Returns the component-wise addition of two integer vectors.
Vector2Int vecAddI(Vector2Int vecA, Vector2Int vecB);
// Returns the component-wise division of two integer vectors. Division is performed with integer truncation.
Vector2Int vecDivI(Vector2Int vecA, Vector2Int vecB);
// Returns the component-wise multiplication of two integer vectors.
Vector2Int vecMulI(Vector2Int vecA, Vector2Int vecB);
// Rounds a floating-point vector to the nearest integer vector using a +0.5 bias.
Vector2Int vecRound(Vector2Float vec);
// Floors a floating-point vector to an integer vector, with negative values rounded down toward negative infinity.
Vector2Int vecFloor(Vector2Float vec);

// Returns true when the point lies within the rectangle defined by cornerA and cornerB, including the borders.
bool isPointInRect(Vector2Int point, Vector2Int cornerA, Vector2Int cornerB);

Vector2Int rectSizeI(Rect2Int rect);

#endif