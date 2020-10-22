#pragma once
#include <iostream>

class EngineBackend
{
public:
	static EngineBackend* getInstance();
	static void initialize();
	static void destroy();

	enum EditorMode {
		EDITOR = 0,
		PLAY = 1,
		PAUSED = 2
	};

	void setMode(EditorMode mode);
	void startFrameStep();
	void endFrameStep();
	bool insideFrameStep();
	EditorMode getMode();

private:
	EngineBackend();
	~EngineBackend();
	EngineBackend(EngineBackend const&) {};             // copy constructor is private
	EngineBackend& operator=(EngineBackend const&) {};  // assignment operator is private*/

	static EngineBackend* sharedInstance;

	EditorMode editorMode = EditorMode::EDITOR;

	bool frameStepping = false;
};

