#include "Debug.h"
#include <iostream>
#include <sstream>

Debug* Debug::sharedInstance = NULL;

void Debug::initialize()
{
	sharedInstance = new Debug();
}

void Debug::destroy()
{
	delete sharedInstance;
}

void Debug::Log(String msg)
{
	if (sharedInstance->console != NULL) {
		sharedInstance->console->appendText(msg);
	}
	else {
		sharedInstance->pendingMessages.push_back(msg);
	}

	std::cout << msg;
}

void Debug::assignConsole(ConsoleScreen* console)
{
	sharedInstance->console = console;
	//print first pending messages
	for (int i = 0; i < sharedInstance->pendingMessages.size(); i++) {
		sharedInstance->console->appendText(sharedInstance->pendingMessages[i]);
	}
	sharedInstance->pendingMessages.clear();
}

Debug::Debug()
{
}

Debug::~Debug()
{
	this->console = NULL;
}
