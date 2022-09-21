#pragma once
#include "Vector3D.h"
#include "Vector2D.h"

class MathUtils
{
public:
	static float randomFloat(float a, float b);
	static int randomInt(int a, int b);
	static float euclideanDist(Vector3D v1, Vector3D v2);
	static float euclideanDist(Vector2D v1, Vector2D v2);
	static float normalize(float x, float xMin, float xMax, float normMin, float normMax);
};

