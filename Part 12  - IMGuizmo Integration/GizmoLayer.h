#pragma once
#include "AUIScreen.h"

class UIManager;
class GizmoLayer :    public AUIScreen
{
private:
	GizmoLayer();
	~GizmoLayer();

	void drawUI() override;
	friend class UIManager;
};

