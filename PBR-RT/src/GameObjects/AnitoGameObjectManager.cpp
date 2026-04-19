#include "AnitoGameObjectManager.h"
#include <algorithm>
#include <iostream>

namespace Anito {

AnitoGameObjectManager* AnitoGameObjectManager::s_instance = nullptr;

AnitoGameObjectManager* AnitoGameObjectManager::getInstance() {
    return s_instance;
}

void AnitoGameObjectManager::initialize() {
    if (s_instance == nullptr) {
        s_instance = new AnitoGameObjectManager();
        std::cout << "AnitoGameObjectManager initialized" << std::endl;
    }
}

void AnitoGameObjectManager::destroy() {
    if (s_instance != nullptr) {
        delete s_instance;
        s_instance = nullptr;
    }
}

AnitoGameObjectManager::AnitoGameObjectManager()
    : m_selectedObject(nullptr)
{
}

AnitoGameObjectManager::~AnitoGameObjectManager() {
    for (AnitoGameObject* obj : m_objectList) {
        delete obj;
    }
    m_objectList.clear();
    m_objectMap.clear();
}

void AnitoGameObjectManager::addObject(AnitoGameObject* obj) {
    if (obj) {
        m_objectList.push_back(obj);
        m_objectMap[obj->getName()] = obj;
        obj->awake();
    }
}

void AnitoGameObjectManager::removeObject(AnitoGameObject* obj) {
    if (obj) {
        removeObjectByName(obj->getName());
    }
}

void AnitoGameObjectManager::removeObjectByName(const String& name) {
    auto it = m_objectMap.find(name);
    if (it != m_objectMap.end()) {
        AnitoGameObject* obj = it->second;

        // Remove from list
        auto listIt = std::find(m_objectList.begin(), m_objectList.end(), obj);
        if (listIt != m_objectList.end()) {
            m_objectList.erase(listIt);
        }

        // Remove from map
        m_objectMap.erase(it);

        // Clear selection if needed
        if (m_selectedObject == obj) {
            m_selectedObject = nullptr;
        }

        delete obj;
    }
}

AnitoGameObject* AnitoGameObjectManager::findObjectByName(const String& name) {
    auto it = m_objectMap.find(name);
    if (it != m_objectMap.end()) {
        return it->second;
    }
    return nullptr;
}

AnitoGameObject* AnitoGameObjectManager::createObject(const String& name, AnitoGameObject::PrimitiveType type) {
    AnitoGameObject* obj = new AnitoGameObject(name, type);
    addObject(obj);
    return obj;
}

void AnitoGameObjectManager::setSelectedObject(AnitoGameObject* obj) {
    m_selectedObject = obj;
}

void AnitoGameObjectManager::setSelectedObject(const String& name) {
    m_selectedObject = findObjectByName(name);
}

void AnitoGameObjectManager::updateAll(float deltaTime) {
    for (AnitoGameObject* obj : m_objectList) {
        obj->update(deltaTime);
    }
}

void AnitoGameObjectManager::renderAll() {
    for (AnitoGameObject* obj : m_objectList) {
        obj->render();
    }
}

} // namespace Anito
