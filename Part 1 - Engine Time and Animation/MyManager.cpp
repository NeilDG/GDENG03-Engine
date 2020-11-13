#include "MyManager.h"

MyManager* MyManager::sharedInstance = nullptr;

MyManager* MyManager::getInstance()
{
	return sharedInstance;
}

void MyManager::initialize()
{
	sharedInstance = new MyManager();
}

void MyManager::destroy()
{
	delete sharedInstance;
}

int MyManager::getX()
{
	return this->x;
}

MyManager::MyManager()
{
}

MyManager::~MyManager()
{
	
}
