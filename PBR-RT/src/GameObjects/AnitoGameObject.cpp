#include "AnitoGameObject.h"
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

namespace Anito {

AnitoGameObject::AnitoGameObject(const String& name, PrimitiveType type)
    : m_name(name)
    , m_objectType(type)
    , m_enabled(true)
    , m_localPosition(AnitoVector3D::zero())
    , m_localRotation(AnitoVector3D::zero())
    , m_localScale(AnitoVector3D::one())
{
    updateLocalMatrix();
}

AnitoGameObject::~AnitoGameObject() {
    for (AnitoComponent* component : m_components) {
        delete component;
    }
    m_components.clear();
}

void AnitoGameObject::awake() {
    for (AnitoComponent* component : m_components) {
        component->awake();
    }
}

void AnitoGameObject::update(float deltaTime) {
    if (!m_enabled) return;

    for (AnitoComponent* component : m_components) {
        component->update(deltaTime);
    }
}

void AnitoGameObject::render() {
    if (!m_enabled) return;

    for (AnitoComponent* component : m_components) {
        component->render();
    }
}

void AnitoGameObject::setPosition(const AnitoVector3D& pos) {
    m_localPosition = pos;
    updateLocalMatrix();
}

void AnitoGameObject::setPosition(float x, float y, float z) {
    setPosition(AnitoVector3D(x, y, z));
}

void AnitoGameObject::setRotation(const AnitoVector3D& rot) {
    m_localRotation = rot;
    updateLocalMatrix();
}

void AnitoGameObject::setRotation(float x, float y, float z) {
    setRotation(AnitoVector3D(x, y, z));
}

void AnitoGameObject::setScale(const AnitoVector3D& scale) {
    m_localScale = scale;
    updateLocalMatrix();
}

void AnitoGameObject::setScale(float x, float y, float z) {
    setScale(AnitoVector3D(x, y, z));
}

void AnitoGameObject::updateLocalMatrix() {
    // Build transformation matrix: T * R * S
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_localPosition.toGLM());
    glm::mat4 rotation = glm::eulerAngleXYZ(m_localRotation.x(), m_localRotation.y(), m_localRotation.z());
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_localScale.toGLM());

    m_localMatrix = AnitoMatrix4x4(translation * rotation * scale);
}

void AnitoGameObject::attachComponent(AnitoComponent* component) {
    if (component) {
        component->attachOwner(this);
        m_components.push_back(component);
        component->awake();
    }
}

void AnitoGameObject::detachComponent(AnitoComponent* component) {
    auto it = std::find(m_components.begin(), m_components.end(), component);
    if (it != m_components.end()) {
        m_components.erase(it);
    }
}

AnitoComponent* AnitoGameObject::findComponentByName(const String& name) {
    for (AnitoComponent* component : m_components) {
        if (component->getName() == name) {
            return component;
        }
    }
    return nullptr;
}

AnitoComponent* AnitoGameObject::findComponentOfType(AnitoComponent::ComponentType type) {
    for (AnitoComponent* component : m_components) {
        if (component->getType() == type) {
            return component;
        }
    }
    return nullptr;
}

AnitoGameObject::ComponentList AnitoGameObject::getComponentsOfType(AnitoComponent::ComponentType type) {
    ComponentList result;
    for (AnitoComponent* component : m_components) {
        if (component->getType() == type) {
            result.push_back(component);
        }
    }
    return result;
}

} // namespace Anito
