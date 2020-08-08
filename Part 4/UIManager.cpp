#include "UIManager.h"

UIManager* UIManager::sharedInstance = NULL;

UIManager* UIManager::getInstance()
{
    return sharedInstance;
}

void UIManager::initialize()
{
    sharedInstance = new UIManager();
}

void UIManager::destroy()
{
    delete sharedInstance;
}

UIManager::UIManager()
{
}

UIManager::~UIManager()
{
}
