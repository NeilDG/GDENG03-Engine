#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "AUIScreen.h"

typedef std::vector<AUIScreen*> UIList;
typedef std::string String;

class UINames {
public:
	const String PROFILER_SCREEN = "";
};

class UIManager
{
public: 
	static UIManager* getInstance();
	static void initialize();
	static void destroy();

	void drawAllUI();

private:
	UIManager();
	~UIManager();
	UIManager(UIManager const&) {};             // copy constructor is private
	UIManager& operator=(UIManager const&) {};  // assignment operator is private*/
	static UIManager* sharedInstance;

	UIList uiList;
};

