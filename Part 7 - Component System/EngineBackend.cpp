#include "EngineBackend.h"

EngineBackend* EngineBackend::sharedInstance = NULL;

EngineBackend* EngineBackend::getInstance()
{
	return sharedInstance;
}

void EngineBackend::initialize()
{
	sharedInstance = new EngineBackend();
}

void EngineBackend::destroy()
{
	delete sharedInstance;
}

void EngineBackend::setMode(EditorMode mode)
{
	this->editorMode = mode;
}

void EngineBackend::startFrameStep()
{
	this->frameStepping = true;
}

void EngineBackend::endFrameStep()
{
	this->frameStepping = false;
}

bool EngineBackend::insideFrameStep()
{
	return this->frameStepping;
}

EngineBackend::EditorMode EngineBackend::getMode()
{
	return this->editorMode;
}

EngineBackend::EngineBackend()
{
}

EngineBackend::~EngineBackend()
{
}
