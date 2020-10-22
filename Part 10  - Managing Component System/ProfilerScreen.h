#pragma once
#include "AUIScreen.h"

class UIManager;
class ProfilerScreen :    public AUIScreen
{
private:
	ProfilerScreen();
	~ProfilerScreen();

	virtual void drawUI() override;
	friend class UIManager;
};

