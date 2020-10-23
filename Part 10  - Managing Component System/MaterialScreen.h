#pragma once
#include "AUIScreen.h"
#include "imfilebrowser.h"
#include "Texture.h"

class InspectorScreen;
class MaterialScreen :    public AUIScreen
{

public:
	void linkInspectorScreen(InspectorScreen* inspectorScreen);
private:
	MaterialScreen();
	~MaterialScreen();

	virtual void drawUI() override;
	friend class UIManager;

	ImGui::FileBrowser* openSceneDialog;
	InspectorScreen* inspectorScreen;

	Texture* displayTex;
	
};

