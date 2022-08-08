#include "Viewport.h"

#include "GameObjectManager.h"
#include "InputSystem.h"
#include "MathUtils.h"
#include "SceneCameraHandler.h"

Viewport::Viewport()
{
	InputSystem::getInstance()->addListener(this);
}

Viewport::~Viewport()
{
	InputSystem::getInstance()->removeListener(this);
}

void Viewport::onKeyUp(int key)
{
}

void Viewport::onKeyDown(int key)
{
}

void Viewport::onMouseMove(const Point deltaPos)
{
	//std::cout << "Viewport Mouse movements! X: " << deltaPos.getX() << " Y: " << deltaPos.getY() << std::endl;
}

void Viewport::onLeftMouseDown(const Point deltaPos, const Point currentPt)
{
	std::cout << "Viewport Mouse DOWN! X: " << currentPt.getX() << " Y: " << currentPt.getY() << std::endl;
	Vector3D worldCoord = SceneCameraHandler::getInstance()->screenToWorldCoordinates(currentPt.getX(), currentPt.getY());

	std::cout << "World coordinates! X: " << worldCoord.getX() << " Y: " << worldCoord.getY() << " Z: " <<worldCoord.getZ() << std::endl;

	/*GameObjectManager::List gameObjectList = GameObjectManager::getInstance()->getAllObjects();
	if (gameObjectList.size() == 0)
		return;


	//first implementation --> select nearest
	int bestIndex = 0;
	float distToBeat = MathUtils::euclideanDist(gameObjectList[bestIndex]->getLocalPosition(), worldCoord);

	for(int i = 1; i < gameObjectList.size(); i++)
	{
		const float dist = MathUtils::euclideanDist(gameObjectList[i]->getLocalPosition(), worldCoord);
		if(dist < distToBeat)
		{
			distToBeat = dist;
			bestIndex = i;
		}
	}

	GameObjectManager::getInstance()->setSelectedObject(gameObjectList[bestIndex]);

	//React physics 3D approach
	/*for (int i = 0; i < gameObjectList.size(); i++)
	{
		if(gameObjectList[i]->testObjectSelect(worldCoord))
		{
			GameObjectManager::getInstance()->setSelectedObject(gameObjectList[i]);
			break;
		}
	}*/

	GameObjectManager::getInstance()->createObject(AGameObject::PrimitiveType::CUBE);
	AGameObject* gameObject = GameObjectManager::getInstance()->getLastObject();
	gameObject->setPosition(worldCoord);
}

void Viewport::onLeftMouseUp(const Point deltaPos)
{
}

void Viewport::onRightMouseDown(const Point deltaPos)
{
}

void Viewport::onRightMouseUp(const Point deltaPos)
{
}
