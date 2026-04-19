#include "AnitoComponent.h"

namespace Anito {

AnitoComponent::AnitoComponent(const std::string& name, ComponentType type)
    : m_name(name)
    , m_type(type)
    , m_owner(nullptr)
{
}

AnitoComponent::~AnitoComponent() {
}

} // namespace Anito
