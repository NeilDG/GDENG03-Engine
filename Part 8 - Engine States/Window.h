#pragma once
#define NOMINMAX //to avoid conflict with Windows.h MIN and MAX declarations
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
	virtual void onFocus();
	virtual void onDefocus();

protected:
	Window();
	~Window();
	Window(Window const&) {};             // copy constructor is private
	Window& operator=(Window const&) {};  // assignment operator is private*/
	HWND windowHandle;

	bool createWindow();

private:
	bool runningFlag = false;

};

