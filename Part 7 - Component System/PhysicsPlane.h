#pragma once
#include "PhysicsCube.h"
class PhysicsPlane :    public Cube
{
public:
	PhysicsPlane(String name, bool skipInit = false);
	~PhysicsPlane();
};

