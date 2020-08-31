#pragma once

class UIManager;
class AUIScreen
{
private:
	AUIScreen();
	~AUIScreen();

	virtual void drawUI() = 0;
	friend class UIManager;
};

