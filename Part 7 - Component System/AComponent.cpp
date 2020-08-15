#include "AComponent.h"

AComponent::AComponent(String name, ComponentType type, AGameObject* owner)
{
    this->name = name;
    this->owner = owner;
    this->type = type;
}

AComponent::~AComponent()
{
    this->owner = NULL;
    this->type = NotSet;
}

void AComponent::attachOwner(AGameObject* owner)
{
    this->owner = owner;
}

void AComponent::detachOwner()
{
    //if object owner gets detached. then component must also be deleted.
    delete this;
}

AGameObject* AComponent::getOwner()
{
    return this->owner;
}

AComponent::ComponentType AComponent::getType()
{
    return this->type;
}

AComponent::String AComponent::getName()
{
    return this->name;
}

void AComponent::perform(float deltaTime)
{
}
