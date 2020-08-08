#pragma once
#include "AUIScreen.h"
class InspectorScreen :    public AUIScreen
{
private:
	typedef std::string String;

	InspectorScreen();
	~InspectorScreen();

	void onTransformUpdate();
	virtual void drawUI() override;
	friend class UIManager;

	float positionDisplay[3] = {0.0f, 0.0f, 0.0f};
	float rotationDisplay[3] = {0.0f, 0.0f, 0.0f};
	float scaleDisplay[3] = { 1.0f, 1.0f, 1.0f };

	String objectNameString = "";


};

