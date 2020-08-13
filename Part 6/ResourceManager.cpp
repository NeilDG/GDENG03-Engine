#include "ResourceManager.h"
#include <filesystem>

AResourceManager* AResourceManager::sharedInstance = NULL;

AResourceManager* AResourceManager::getInstance()
{
    return sharedInstance;
}

void AResourceManager::initialize()
{
    sharedInstance = new AResourceManager();
}

void AResourceManager::destroy()
{
    delete sharedInstance;
}

Resource* AResourceManager::createResourceFromFile(const wchar_t* filePath)
{
    std::wstring fullPath = std::filesystem::absolute(filePath);

    if (this->resourceMap[fullPath] == NULL) {
        this->resourceMap[fullPath] = this->createResourceFromFile(filePath);
    }
    return this->resourceMap[fullPath];
}

AResourceManager::AResourceManager()
{
}

AResourceManager::~AResourceManager()
{
}
