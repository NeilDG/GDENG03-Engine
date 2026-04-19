#pragma once
// Transform component - handles object positioning

#include "../GameObjects/AnitoComponent.h"
#include "../Math/AnitoVector3D.h"
#include "../Math/AnitoMatrix4x4.h"

namespace Anito {

/**
 * AnitoTransform - Transform component
 * Every GameObject has a transform
 */
class AnitoTransform : public AnitoComponent {
public:
    AnitoTransform(const std::string& name = "Transform");
    ~AnitoTransform() override;

    void setPosition(const AnitoVector3D& pos);
    AnitoVector3D getPosition() const { return m_position; }

    void setRotation(const AnitoVector3D& rot);
    AnitoVector3D getRotation() const { return m_rotation; }

    void setScale(const AnitoVector3D& scale);
    AnitoVector3D getScale() const { return m_scale; }

    AnitoMatrix4x4 getMatrix() const { return m_matrix; }
    void updateMatrix();

private:
    AnitoVector3D m_position;
    AnitoVector3D m_rotation;
    AnitoVector3D m_scale;
    AnitoMatrix4x4 m_matrix;
};

} // namespace Anito
