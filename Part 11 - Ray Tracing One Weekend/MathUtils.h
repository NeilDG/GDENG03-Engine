#pragma once
#include <cmath>
#include <limits>
#include <memory>

class MathUtils
{
public:
	static float infinity();
	static float degreesToRadians(float degrees);
	static float randomFloat();
	static float randomFloat(float min, float max);
};

