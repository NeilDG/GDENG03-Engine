#include <iostream>
#include "AppWindow.h"
int main() 
{
	int count = 0;
	for(int i = 1; i < 5; i++)
	{
		for (int j = i; j < 5; j++)
		{
			count++;
			std::cout << "My algorithm runs! " << (count) << std::endl;
		}
	}
	
	/*AppWindow::initialize();
	AppWindow* runningApp = (AppWindow*) AppWindow::getInstance();
	runningApp->initializeEngine();
	runningApp->createInterface();

	while (runningApp->isRunning()) {
		runningApp->broadcast();
	}*/
	
}