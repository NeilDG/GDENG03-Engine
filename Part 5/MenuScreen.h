#pragma once
#include "AUIScreen.h"
class MenuScreen :    public AUIScreen
{
private:
	MenuScreen();
	~MenuScreen();

	virtual void drawUI() override;
	friend class UIManager;

	bool isOpen = false;
};

