#pragma once
// Base GameObject class

#include "AnitoComponent.h"
#include "../Math/AnitoVector3D.h"
#include "../Math/AnitoMatrix4x4.h"
#include <string>
#include <vector>
#include <memory>

namespace Anito {

/**
 * AnitoGameObject - Base game object class
 * Component-based architecture following reference implementation
 */
class AnitoGameObject {
public:
    enum class PrimitiveType {
        Empty,
        Cube,
        Sphere,
        Plane,
        Capsule,
        Cylinder,
        Camera,
        Light
    };

    typedef std::string String;
    typedef std::vector<AnitoComponent*> ComponentList;

    AnitoGameObject(const String& name, PrimitiveType type = PrimitiveType::Empty);
    virtual ~AnitoGameObject();

    // Lifecycle
    virtual void awake();
    virtual void update(float deltaTime);
    virtual void render();

    // Transform accessors
    void setPosition(const AnitoVector3D& pos);
    void setPosition(float x, float y, float z);
    AnitoVector3D getPosition() const { return m_localPosition; }

    void setRotation(const AnitoVector3D& rot);
    void setRotation(float x, float y, float z);
    AnitoVector3D getRotation() const { return m_localRotation; }

    void setScale(const AnitoVector3D& scale);
    void setScale(float x, float y, float z);
    AnitoVector3D getScale() const { return m_localScale; }

    // Matrix management
    void updateLocalMatrix();
    AnitoMatrix4x4 getLocalMatrix() const { return m_localMatrix; }
    const float* getMatrixData() const { return m_localMatrix.data(); }

    // Component system
    void attachComponent(AnitoComponent* component);
    void detachComponent(AnitoComponent* component);
    AnitoComponent* findComponentByName(const String& name);
    AnitoComponent* findComponentOfType(AnitoComponent::ComponentType type);
    ComponentList getComponentsOfType(AnitoComponent::ComponentType type);

    // Template helper for getting typed components
    template<typename T>
    T* getComponent() {
        for (AnitoComponent* component : m_components) {
            T* typed = dynamic_cast<T*>(component);
            if (typed != nullptr) {
                return typed;
            }
        }
        return nullptr;
    }

    // Properties
    String getName() const { return m_name; }
    PrimitiveType getObjectType() const { return m_objectType; }

    bool isEnabled() const { return m_enabled; }
    void setEnabled(bool enabled) { m_enabled = enabled; }

protected:
    String m_name;
    PrimitiveType m_objectType;
    bool m_enabled;

    AnitoVector3D m_localPosition;
    AnitoVector3D m_localRotation; // Euler angles in radians
    AnitoVector3D m_localScale;
    AnitoMatrix4x4 m_localMatrix;

    ComponentList m_components;
};

} // namespace Anito
