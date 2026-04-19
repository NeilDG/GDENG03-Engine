#pragma once
// Base component class

#include <string>

namespace Anito {

class AnitoGameObject;

/**
 * AnitoComponent - Base class for all components
 * Similar to Unity/Unreal component architecture
 */
class AnitoComponent {
public:
    enum class ComponentType {
        NotSet,
        Transform,
        MeshRenderer,
        Camera,
        Light,
        RigidBody,
        Collider,
        Script
    };

    AnitoComponent(const std::string& name, ComponentType type);
    virtual ~AnitoComponent();

    virtual void awake() {}
    virtual void update(float deltaTime) {}
    virtual void render() {}

    void attachOwner(AnitoGameObject* owner) { m_owner = owner; }
    AnitoGameObject* getOwner() const { return m_owner; }

    std::string getName() const { return m_name; }
    ComponentType getType() const { return m_type; }

protected:
    std::string m_name;
    ComponentType m_type;
    AnitoGameObject* m_owner;
};

} // namespace Anito
