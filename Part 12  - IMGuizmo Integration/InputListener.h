#pragma once
#include "Point.h"

class InputListener {
protected:
	~InputListener() = default;
public:
	virtual void onKeyUp(int key) = 0;
	virtual void onKeyDown(int key) = 0;

	virtual void onMouseMove(const Point deltaPos) = 0;

	virtual void onLeftMouseDown(const Point deltaPos, const Point currentPt) = 0;
	virtual void onLeftMouseUp(const Point deltaPos, const Point currentPt) = 0;

	virtual void onRightMouseDown(const Point deltaPos) = 0;
	virtual void onRightMouseUp(const Point deltaPos) = 0;
};
