#pragma once
#include "AUIScreen.h"
#include "imfilebrowser.h"
class MaterialScreen :    public AUIScreen
{
private:
	MaterialScreen();
	~MaterialScreen();

	virtual void drawUI() override;
	friend class UIManager;

	ImGui::FileBrowser* openSceneDialog;
};

