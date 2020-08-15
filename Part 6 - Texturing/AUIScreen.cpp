#include "AUIScreen.h"

String AUIScreen::getName()
{
	return this->name;
}

AUIScreen::AUIScreen(String name)
{
	this->name = name;
}

AUIScreen::~AUIScreen()
{
}
