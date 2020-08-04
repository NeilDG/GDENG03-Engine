#pragma once
#include <iostream>
#include "InputListener.h"
#include <Windows.h>
#include <vector>


//Also added support for direct checking of keyboard/mouse events here, like Unity Engine.
//By: Neil DG

typedef std::vector<InputListener*> List;

class InputSystem
{
public:
	static InputSystem* getInstance();
	static void initialize();
	static void destroy();

	void addListener(InputListener* listener);
	void removeListener(InputListener* listener);
	void update();

	bool isKeyDown(int key);
	bool isKeyUp(int key);

private:
	InputSystem();
	~InputSystem();
	InputSystem(InputSystem const&) {};             // copy constructor is private
	InputSystem& operator=(InputSystem const&) {};  // assignment operator is private*/

	void callOnKeyDown(int key);
	void callOnKeyUp(int key);

	static InputSystem* sharedInstance;
	List inputListenerList;
	unsigned char keyStates[256] = {};
	unsigned char oldKeyStates[256] = {};
	
};

