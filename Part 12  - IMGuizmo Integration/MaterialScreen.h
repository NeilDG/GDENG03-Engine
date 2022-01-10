#pragma once
#include "AUIScreen.h"
#include "imfilebrowser.h"
#include "Texture.h"

class InspectorScreen;
class MaterialScreen :    public AUIScreen
{

public:
	void linkInspectorScreen(InspectorScreen* inspectorScreen, String materialPath);
	
private:
	MaterialScreen();
	~MaterialScreen();

	virtual void drawUI() override;
	friend class UIManager;

	ImGui::FileBrowser* openSceneDialog;
	InspectorScreen* inspectorScreen;

	String materialPath;
	Texture* displayTex;
	
};

