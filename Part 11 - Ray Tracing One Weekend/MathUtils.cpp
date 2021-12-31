#include "MathUtils.h"

static const float PI = 3.1415926535897932385f;

float MathUtils::infinity()
{
	const float infinity = std::numeric_limits<float>::infinity();
	return infinity;
}

float MathUtils::degreesToRadians(float degrees)
{
	return degrees * PI / 180.0f;
}

float MathUtils::randomFloat()
{
	return rand() / (RAND_MAX + 1.0f);
}

float MathUtils::randomFloat(float min, float max)
{
	return min + (max - min) * randomFloat();
}

