#include "MathUtils.h"
#include <stdlib.h>
#include <cmath>

float MathUtils::randomFloat(float a, float b)
{
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

int MathUtils::randomInt(int a, int b)
{
    int range = a - b + 1;
    int num = rand() % range + a;

    return num;
}

float MathUtils::euclideanDist(Vector3D q, Vector3D p)
{
    float xpow = powf(q.getX() - p.getX(), 2);
    float ypow = powf(q.getY() - p.getY(), 2);
    float zpow = powf(q.getZ() - p.getZ(), 2);

    float result = sqrt(xpow + ypow + zpow);
    return result;
}

float MathUtils::euclideanDist(Vector2D q, Vector2D p)
{
    float xpow = powf(q.getX() - p.getX(), 2);
    float ypow = powf(q.getY() - p.getY(), 2);

    float result = sqrt(xpow + ypow);
    return result;
}

float MathUtils::normalize(float x, float xMin, float xMax, float normMin, float normMax)
{
    float normRange = (normMax - normMin);
    float xNorm = (x - xMin) / (xMax - xMin);
    float result = (normRange * xNorm) + normMin;

    return result;
}
