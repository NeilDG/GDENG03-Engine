#pragma once
#include "AGameObject.h"

class GizmoLayer
{
public:
	static GizmoLayer* getInstance();
	static void initialize();
	static void destroy();

	void draw();
	void enableGizmo(AGameObject* selectedObject);

private:
	GizmoLayer();
	~GizmoLayer();
	GizmoLayer(GizmoLayer const&) {};
	GizmoLayer& operator=(GizmoLayer const&) {};
	static GizmoLayer* sharedInstance;

	bool gridShow = true;
	bool translate = true;
	bool rotate = false;
	bool scale = false;
	AGameObject* selectedObject = nullptr;

	
};

