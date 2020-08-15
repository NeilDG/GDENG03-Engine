#include "PhysicsSystem.h"
#include "PhysicsComponent.h"
#include <iostream>
#include "EngineTime.h"

PhysicsSystem::PhysicsSystem()
{
}

PhysicsSystem::~PhysicsSystem()
{
}

void PhysicsSystem::registerComponent(PhysicsComponent* component)
{
    this->componentTable[component->getName()] = component;
    this->componentList.push_back(component);
}

void PhysicsSystem::unregisterComponent(PhysicsComponent* component)
{
    if (this->componentTable[component->getName()] != NULL) {
        this->componentTable.erase(component->getName());
		int index = -1;
		for (int i = 0; i < this->componentList.size(); i++) {
			if (this->componentList[i] == component) {
				index = i;
				break;
			}
		}

		if (index != -1) {
			this->componentList.erase(this->componentList.begin() + index);
		}
    }
	else {
		std::cout << "Component " << component->getName() << " not registered in physics component. \n";
	}
}

void PhysicsSystem::unregisterComponentByName(String name)
{
	if (this->componentTable[name] != NULL) {
		this->unregisterComponent(this->componentTable[name]);
	}
}

PhysicsComponent* PhysicsSystem::findComponentByName(String name)
{
	if (this->componentTable[name] == NULL) {
		std::cout << "Component " << name << " not registered in physics component. \n";
	}

	return this->componentTable[name];
}

PhysicsSystem::ComponentList PhysicsSystem::getAllComponents()
{
    return this->componentList;
}

void PhysicsSystem::updateAllComponents()
{
	for (int i = 0; i < this->componentList.size(); i++) {
		this->componentList[i]->perform(EngineTime::getDeltaTime());
	}
}
