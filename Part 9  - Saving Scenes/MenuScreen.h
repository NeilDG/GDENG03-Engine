#pragma once
#include "AUIScreen.h"
#include "imfilebrowser.h"

class MenuScreen :    public AUIScreen
{
private:
	MenuScreen();
	~MenuScreen();

	virtual void drawUI() override;
	void OnCreateCubeClicked();
	void OnCreateTexturedCubeClicked();
	void OnCreateSphereClicked();
	void OnCreatePlaneClicked();

	void OnRigidBodyComponentClicked();
	void OnPhysicsCubeClicked();
	void OnPhysicsPlaneClicked();

	friend class UIManager;

	bool isOpen = false;

	bool openSceneSelected = false;

	ImGui::FileBrowser* saveSceneDialog;
	ImGui::FileBrowser* openSceneDialog;
};

