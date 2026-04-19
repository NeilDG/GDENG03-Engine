#pragma once
// Lighting system - supports directional, point, and spot lights

#include "../../Math/AnitoVector3D.h"
#include "../../Math/AnitoVector4D.h"
#include <string>

namespace Anito {

/**
 * AnitoLight - Base light class
 * Supports PBR lighting with shadows
 */
class AnitoLight {
public:
    enum class LightType {
        Directional,
        Point,
        Spot
    };

    AnitoLight(LightType type);
    virtual ~AnitoLight();

    LightType getType() const { return m_type; }

    void setColor(const AnitoVector3D& color) { m_color = color; }
    AnitoVector3D getColor() const { return m_color; }

    void setIntensity(float intensity) { m_intensity = intensity; }
    float getIntensity() const { return m_intensity; }

    void setCastShadows(bool castShadows) { m_castShadows = castShadows; }
    bool getCastShadows() const { return m_castShadows; }

protected:
    LightType m_type;
    AnitoVector3D m_color;
    float m_intensity;
    bool m_castShadows;
};

/**
 * AnitoDirectionalLight - Sun/moon light
 */
class AnitoDirectionalLight : public AnitoLight {
public:
    AnitoDirectionalLight();

    void setDirection(const AnitoVector3D& direction) { m_direction = direction.normalized(); }
    AnitoVector3D getDirection() const { return m_direction; }

private:
    AnitoVector3D m_direction;
};

/**
 * AnitoPointLight - Omnidirectional light with attenuation
 */
class AnitoPointLight : public AnitoLight {
public:
    AnitoPointLight();

    void setPosition(const AnitoVector3D& position) { m_position = position; }
    AnitoVector3D getPosition() const { return m_position; }

    void setRadius(float radius) { m_radius = radius; }
    float getRadius() const { return m_radius; }

private:
    AnitoVector3D m_position;
    float m_radius;
};

/**
 * AnitoSpotLight - Cone-shaped light
 */
class AnitoSpotLight : public AnitoLight {
public:
    AnitoSpotLight();

    void setPosition(const AnitoVector3D& position) { m_position = position; }
    AnitoVector3D getPosition() const { return m_position; }

    void setDirection(const AnitoVector3D& direction) { m_direction = direction.normalized(); }
    AnitoVector3D getDirection() const { return m_direction; }

    void setInnerAngle(float angle) { m_innerAngle = angle; }
    float getInnerAngle() const { return m_innerAngle; }

    void setOuterAngle(float angle) { m_outerAngle = angle; }
    float getOuterAngle() const { return m_outerAngle; }

    void setRadius(float radius) { m_radius = radius; }
    float getRadius() const { return m_radius; }

private:
    AnitoVector3D m_position;
    AnitoVector3D m_direction;
    float m_innerAngle;
    float m_outerAngle;
    float m_radius;
};

} // namespace Anito
