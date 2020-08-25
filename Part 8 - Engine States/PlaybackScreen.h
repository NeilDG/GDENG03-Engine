#pragma once
#include "AUIScreen.h"
class PlaybackScreen :    public AUIScreen
{
private:
	PlaybackScreen();
	~PlaybackScreen();

	virtual void drawUI() override;
	friend class UIManager;

	bool played = false;
	bool paused = false;

	float colors[3] = { 0.0f, 0.0f, 0.0f };
};

