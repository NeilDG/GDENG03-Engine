#include "AnitoLight.h"

namespace Anito {

AnitoLight::AnitoLight(LightType type)
    : m_type(type)
    , m_color(AnitoVector3D(1.0f, 1.0f, 1.0f))
    , m_intensity(1.0f)
    , m_castShadows(true)
{
}

AnitoLight::~AnitoLight() {
}

AnitoDirectionalLight::AnitoDirectionalLight()
    : AnitoLight(LightType::Directional)
    , m_direction(AnitoVector3D(0.0f, -1.0f, 0.0f))
{
}

AnitoPointLight::AnitoPointLight()
    : AnitoLight(LightType::Point)
    , m_position(AnitoVector3D::zero())
    , m_radius(10.0f)
{
}

AnitoSpotLight::AnitoSpotLight()
    : AnitoLight(LightType::Spot)
    , m_position(AnitoVector3D::zero())
    , m_direction(AnitoVector3D(0.0f, -1.0f, 0.0f))
    , m_innerAngle(0.785f) // 45 degrees
    , m_outerAngle(1.047f) // 60 degrees
    , m_radius(10.0f)
{
}

} // namespace Anito
