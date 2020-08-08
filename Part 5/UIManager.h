#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "Window.h"
#include "AUIScreen.h"

typedef std::string String;
typedef std::vector<AUIScreen*> UIList;
typedef std::map<String, AUIScreen*> UITable;

class UINames {
public:
	const String PROFILER_SCREEN = "PROFILER_SCREEN";
	const String MENU_SCREEN = "MENU_SCREEN";
};


class UIManager
{
public: 
	static UIManager* getInstance();
	static void initialize(HWND windowHandle);
	static void destroy();

	void drawAllUI();

private:
	UIManager(HWND windowHandle);
	~UIManager();
	UIManager(UIManager const&) {};             // copy constructor is private
	UIManager& operator=(UIManager const&) {};  // assignment operator is private*/
	static UIManager* sharedInstance;

	UIList uiList;
	UITable uiTable;
};

