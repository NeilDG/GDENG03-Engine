#pragma once
#include "Point.h"

class InputListener {

public:
	virtual void onKeyUp(int key) = 0;
	virtual void onKeyDown(int key) = 0;

	virtual void onMouseMove(const Point deltaPos) = 0;

	virtual void onLeftMouseDown(const Point deltaPos) = 0;
	virtual void onLeftMouseUp(const Point deltaPos) = 0;

	virtual void onRightMouseDown(const Point deltaPos) = 0;
	virtual void onRightMouseUp(const Point deltaPos) = 0;
};
