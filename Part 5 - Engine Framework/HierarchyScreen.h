#pragma once
#include "AUIScreen.h"
class HierarchyScreen :    public AUIScreen
{
private:
	HierarchyScreen();
	~HierarchyScreen();

	virtual void drawUI() override;
	void updateObjectList() const;
	friend class UIManager;
};

