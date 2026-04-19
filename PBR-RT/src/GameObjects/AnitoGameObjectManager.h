#pragma once
// GameObject Manager - manages all game objects in the scene

#include "AnitoGameObject.h"
#include <vector>
#include <unordered_map>
#include <string>

namespace Anito {

/**
 * AnitoGameObjectManager - Singleton manager for all game objects
 * Similar to Unity's scene management
 */
class AnitoGameObjectManager {
public:
    typedef std::string String;
    typedef std::vector<AnitoGameObject*> GameObjectList;
    typedef std::unordered_map<String, AnitoGameObject*> GameObjectMap;

    static AnitoGameObjectManager* getInstance();
    static void initialize();
    static void destroy();

    // Object management
    void addObject(AnitoGameObject* obj);
    void removeObject(AnitoGameObject* obj);
    void removeObjectByName(const String& name);

    AnitoGameObject* findObjectByName(const String& name);
    GameObjectList getAllObjects() const { return m_objectList; }
    int getObjectCount() const { return static_cast<int>(m_objectList.size()); }

    // Object creation
    AnitoGameObject* createObject(const String& name, AnitoGameObject::PrimitiveType type);

    // Selection
    void setSelectedObject(AnitoGameObject* obj);
    void setSelectedObject(const String& name);
    AnitoGameObject* getSelectedObject() const { return m_selectedObject; }

    // Update/Render all
    void updateAll(float deltaTime);
    void renderAll();

private:
    AnitoGameObjectManager();
    ~AnitoGameObjectManager();
    AnitoGameObjectManager(const AnitoGameObjectManager&) = delete;
    AnitoGameObjectManager& operator=(const AnitoGameObjectManager&) = delete;

    static AnitoGameObjectManager* s_instance;

    GameObjectList m_objectList;
    GameObjectMap m_objectMap;
    AnitoGameObject* m_selectedObject;
};

} // namespace Anito
