#include "InputSystem.h"

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
	if(GetKeyboardState(this->keyStates)) {
		for (int i = 0; i < ARRAYSIZE(this->keyStates); i++) {
			if (this->keyStates[i] & 0x80 && this->keyStates[i] != this->oldKeyStates[i]) {
				this->callOnKeyDown(i);
			}
			else if(this->keyStates[i] != this->oldKeyStates[i]) {
				this->callOnKeyUp(i);
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

InputSystem::InputSystem()
{
}

InputSystem::~InputSystem()
{
	this->inputListenerList.clear();
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
