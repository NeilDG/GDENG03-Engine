#pragma once
#include <string>
#include "InputListener.h"
/**
 * Represents the whole viewport for ray-casting mouse to picking 3D objects
 */
class Viewport: InputListener
{
public:
	typedef std::string String;

	Viewport();
	~Viewport();

	void onKeyUp(int key) override;
	void onKeyDown(int key) override;
	void onMouseMove(const Point deltaPos) override;
	void onLeftMouseDown(const Point deltaPos, const Point currentPt) override;
	void onLeftMouseUp(const Point deltaPos) override;
	void onRightMouseDown(const Point deltaPos) override;
	void onRightMouseUp(const Point deltaPos) override;
};

