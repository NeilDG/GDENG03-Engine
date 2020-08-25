#include "Vector2D.h"

Vector2D::Vector2D()
{
}

Vector2D::~Vector2D()
{
}

Vector2D::Vector2D(float x, float y)
{
	this->x = x;
	this->y = y;
}

Vector2D::Vector2D(const Vector2D* vector)
{
	this->x = vector->x;
	this->y = vector->y;
}

float Vector2D::getX() const
{
	return this->x;
}

float Vector2D::getY() const
{
	return this->y;
}

Vector2D Vector2D::operator*(float num)
{
	return Vector2D(this->x * num, this->y * num);
}

Vector2D Vector2D::operator+(Vector2D vec)
{
	return Vector2D(this->x + vec.x, this->y + vec.y);
}

Vector2D Vector2D::lerp(const Vector2D start, const Vector2D end, float delta)
{
	Vector2D v;
	v.x = start.x * (1.0f - delta) + end.x * (delta);
	v.y = start.y * (1.0f - delta) + end.y * (delta);

	return v;
}

Vector2D Vector2D::zeros()
{
	return Vector2D(0.0f, 0.0f);
}

Vector2D Vector2D::ones()
{
	return Vector2D(1.0f, 1.0f);
}
