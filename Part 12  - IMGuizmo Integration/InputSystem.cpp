#include "InputSystem.h"

#include "AppWindow.h"

InputSystem* InputSystem::sharedInstance = NULL;

InputSystem* InputSystem::getInstance()
{
	return sharedInstance;
}

void InputSystem::initialize()
{
	sharedInstance = new InputSystem();
}

void InputSystem::destroy()
{
	if (sharedInstance != NULL) {
		delete sharedInstance;
	}
}

InputSystem::InputSystem()
{
	this->oldMousePos = Point(0, 0);
}

InputSystem::~InputSystem()
{
	this->inputListenerList.clear();
}

void InputSystem::addListener(InputListener* listener)
{
	this->inputListenerList.push_back(listener);
}

void InputSystem::removeListener(InputListener* listener)
{
	//find object in used
	int index = -1;
	for (int i = 0; i < this->inputListenerList.size(); i++) {
		if (this->inputListenerList[i] == listener) {
			index = i;
			break;
		}
	}
	if (index > -1) {
		this->inputListenerList.erase(this->inputListenerList.begin() + index);
	}
}

void InputSystem::update()
{
	
	POINT currentPt = {};
	GetCursorPos(&currentPt);
	ScreenToClient(AppWindow::getInstance()->getWindowHandle(), &currentPt);

	if (this->firstTimeCall) {
		this->firstTimeCall = false;
		this->oldMousePos = Point(currentPt.x, currentPt.y);
	}

	if (this->oldMousePos.getX() != currentPt.x || this->oldMousePos.getY() != currentPt.y) {
		Point deltaPt = Point(currentPt.x - this->oldMousePos.getX(), currentPt.y - this->oldMousePos.getY());
		this->callOnMouseMove(deltaPt);
	}

	this->oldMousePos = Point(currentPt.x, currentPt.y);

	if(GetKeyboardState(this->keyStates)) {
		for (int i = 0; i < ARRAYSIZE(this->keyStates); i++) {
			if (this->keyStates[i] & 0x80 && this->keyStates[i] != this->oldKeyStates[i]) {
				if (i == VK_LBUTTON && this->keyStates[i] != this->oldKeyStates[i]) {
					Point deltaPt = Point(currentPt.x - this->oldMousePos.getX(), currentPt.y - this->oldMousePos.getY());
					Point pt = Point(currentPt.x, currentPt.y);
					this->callOnLeftMouseDown(deltaPt, pt);
				}
				else if (i == VK_RBUTTON && this->keyStates[i] != this->oldKeyStates[i]) {
					Point deltaPt = Point(currentPt.x - this->oldMousePos.getX(), currentPt.y - this->oldMousePos.getY());
					this->callOnRightMouseDown(deltaPt);
				}
				else {
					this->callOnKeyDown(i);
				}
			}
			else if(this->keyStates[i] != this->oldKeyStates[i]) {
				if (i == VK_LBUTTON && this->keyStates[i] != this->oldKeyStates[i]) {
					Point deltaPt = Point(currentPt.x - this->oldMousePos.getX(), currentPt.y - this->oldMousePos.getY());
					this->callOnLeftMouseUp(deltaPt);
				}
				else if (i == VK_RBUTTON && this->keyStates[i] != this->oldKeyStates[i]) {
					Point deltaPt = Point(currentPt.x - this->oldMousePos.getX(), currentPt.y - this->oldMousePos.getY());
					this->callOnRightMouseUp(deltaPt);
				}
				else {
					this->callOnKeyUp(i);
				}
			}
		}

		//store current keys to old states
		::memcpy(this->oldKeyStates, this->keyStates, sizeof(unsigned char) * 256);
	}
}

bool InputSystem::isKeyDown(int key)
{
	for (int i = 0; i < ARRAYSIZE(this->keyStates); i++) {
		if (this->keyStates[i] & 0x80 && i == key) {
			return true;
		}
		else if (i == key) {
			return false;
		}
	}
	

	return false;
}

bool InputSystem::isKeyUp(int key)
{
	for (int i = 0; i < ARRAYSIZE(this->keyStates); i++) {
		if (!(this->keyStates[i] & 0x80) && i == key) {
			return true;
		}
		else if (i == key) {
			return false;
		}
	}

	return false;
}


void InputSystem::callOnKeyDown(int key)
{
	for (int i = 0; i < this->inputListenerList.size(); i++) {
		this->inputListenerList[i]->onKeyDown(key);
	}
}

void InputSystem::callOnKeyUp(int key)
{
	for (int i = 0; i < this->inputListenerList.size(); i++) {
		this->inputListenerList[i]->onKeyUp(key);
	}
}

void InputSystem::callOnMouseMove(Point deltaPt)
{
	for (int i = 0; i < this->inputListenerList.size(); i++) {
		this->inputListenerList[i]->onMouseMove(deltaPt);
	}
}

void InputSystem::callOnLeftMouseDown(Point deltaPt, Point currentPt)
{
	for (int i = 0; i < this->inputListenerList.size(); i++) {
		this->inputListenerList[i]->onLeftMouseDown(deltaPt, currentPt);
	}
}

void InputSystem::callOnLeftMouseUp(Point deltaPt)
{
	for (int i = 0; i < this->inputListenerList.size(); i++) {
		this->inputListenerList[i]->onLeftMouseUp(deltaPt);
	}
}

void InputSystem::callOnRightMouseDown(Point deltaPt)
{
	for (int i = 0; i < this->inputListenerList.size(); i++) {
		this->inputListenerList[i]->onRightMouseDown(deltaPt);
	}
}

void InputSystem::callOnRightMouseUp(Point deltaPt)
{
	for (int i = 0; i < this->inputListenerList.size(); i++) {
		this->inputListenerList[i]->onRightMouseUp(deltaPt);
	}
}
