#include "Point.h"

Point::Point()
{
}

Point::Point(int x, int y)
{
	this->x = x;
	this->y = y;
}

Point::Point(const Point* point)
{
	this->x = point->x;
	this->y = point->y;
}

Point::~Point()
{
	
}

int Point::getX() const
{
	return this->x;
}

int Point::getY() const
{
	return this->y;
}
