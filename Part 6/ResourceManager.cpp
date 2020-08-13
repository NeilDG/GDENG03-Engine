#include "ResourceManager.h"
#include <filesystem>

/*Resource* AResourceManager::createResourceFromFile(const wchar_t* filePath)
{
    std::wstring fullPath = std::filesystem::absolute(filePath);

    if (this->resourceMap[fullPath] == NULL) {
        this->resourceMap[fullPath] = this->createResourceFromFileConcrete(filePath);
    }
    return this->resourceMap[fullPath];
}*/

AResourceManager::AResourceManager()
{
}

AResourceManager::~AResourceManager()
{
    this->resourceMap.clear();
}
