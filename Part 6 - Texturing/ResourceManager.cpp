#include "ResourceManager.h"
#include <filesystem>

AResourceManager::AResourceManager()
{
}

AResourceManager::~AResourceManager()
{
    if (this->resourceMap.size() != 0) {
        this->resourceMap.clear();
    }
}
