#pragma once
#include "AUIScreen.h"
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

	friend class UIManager;

	bool isOpen = false;
};

