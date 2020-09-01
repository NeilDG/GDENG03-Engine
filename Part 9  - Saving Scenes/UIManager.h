#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "Window.h"
#include "AUIScreen.h"

class UINames {
public:
	const String PROFILER_SCREEN = "PROFILER_SCREEN";
	const String MENU_SCREEN = "MENU_SCREEN";
	const String INSPECTOR_SCREEN = "INSPECTOR_SCREEN";
	const String HIERARCHY_SCREEN = "HIERARCHY_SCREEN";
	const String PLAYBACK_SCREEN = "PLAYBACK_SCREEN";
	const String ACTION_SCREEN = "ACTION_SCREEN";
};


class UIManager
{
public: 
	typedef std::string String;
	typedef std::vector<AUIScreen*> UIList;
	typedef std::unordered_map<String, AUIScreen*> UITable;

	static UIManager* getInstance();
	static void initialize(HWND windowHandle);
	static void destroy();

	void drawAllUI();

	static const int WINDOW_WIDTH = 1440;
	static const int WINDOW_HEIGHT = 900;

private:
	UIManager(HWND windowHandle);
	~UIManager();
	UIManager(UIManager const&) {};             // copy constructor is private
	UIManager& operator=(UIManager const&) {};  // assignment operator is private*/
	static UIManager* sharedInstance;

	UIList uiList;
	UITable uiTable;
};

