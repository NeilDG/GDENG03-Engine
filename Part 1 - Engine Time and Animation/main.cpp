#include <iostream>
#include "AppWindow.h"
int main() 
{
	AppWindow::initialize();
	AppWindow* runningApp = (AppWindow*) AppWindow::getInstance();
	runningApp->initializeEngine();

	while (runningApp->isRunning()) {
		runningApp->broadcast();
	}

}