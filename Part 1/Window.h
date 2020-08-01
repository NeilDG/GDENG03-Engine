#pragma once
#include <Windows.h>
#include <iostream>

class Window
{
public:
	bool broadcast();
	bool release();
	bool isRunning();

	RECT getClientWindowRect();
	HWND getWindowHandle();

	//custom events
	virtual void onCreate();
	virtual void onUpdate();
	virtual void onDestroy();

protected:
	Window();
	~Window();
	Window(Window const&) {};             // copy constructor is private
	Window& operator=(Window const&) {};  // assignment operator is private*/

	bool createWindow();

private:
	HWND windowHandle;
	bool runningFlag = false;

};

