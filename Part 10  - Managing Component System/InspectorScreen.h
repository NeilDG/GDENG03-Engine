#pragma once
#include "AUIScreen.h"

class AGameObject;
class Texture;
class InspectorScreen :    public AUIScreen
{

public:
	void SendResult(String materialPath);
	
private:
	InspectorScreen();
	~InspectorScreen();

	void onTransformUpdate();
	virtual void drawUI() override;
	void updateTransformDisplays();
	void FormatMatImage();
	void drawComponentsTab();
	friend class UIManager;

	float positionDisplay[3] = {0.0f, 0.0f, 0.0f};
	float rotationDisplay[3] = {0.0f, 0.0f, 0.0f};
	float scaleDisplay[3] = { 1.0f, 1.0f, 1.0f };
	bool popupOpen = false;
	
	AGameObject* selectedObject = NULL;
	const String DEFAULT_MATERIAL = "None";
	String materialPath = DEFAULT_MATERIAL;
	String materialName = DEFAULT_MATERIAL;
	Texture* materialDisplay;
	
	
};

