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
	//std::cout << "Viewport Mouse DOWN! X: " << currentPt.getX() << " Y: " << currentPt.getY() << std::endl;
	//Vector3D worldCoord = SceneCameraHandler::getInstance()->screenToWorldCoordinates(currentPt.getX(), currentPt.getY());
	//std::cout << "World coordinates! X: " << worldCoord.getX() << " Y: " << worldCoord.getY() << " Z: " <<worldCoord.getZ() << std::endl;

	const GameObjectManager::List gameObjectList = GameObjectManager::getInstance()->getAllObjects();
	if (gameObjectList.empty())
		return;


	//first implementation --> select nearest
	int bestIndex = 0;

	Vector2D screenPos = SceneCameraHandler::getInstance()->worldToScreenCoordinates(gameObjectList[bestIndex]->getLocalPosition());
	Vector2D mousePos = Vector2D(currentPt.getX(), currentPt.getY());
	float distToBeat = MathUtils::euclideanDist(screenPos, mousePos);

	for (int i = 1; i < gameObjectList.size(); i++)
	{
		Vector2D screenPos = SceneCameraHandler::getInstance()->worldToScreenCoordinates(gameObjectList[i]->getLocalPosition());
		const float dist = MathUtils::euclideanDist(screenPos, mousePos);
		std::cout << "Distance of object " << i << ": " << dist << std::endl;
		std::cout << "Mouse Pos: " << mousePos.getX() << " " << mousePos.getY() <<
			" Screen Pos: " << screenPos.getX() << " " << screenPos.getY() <<
			" Distance to beat: " << distToBeat << std::endl;
		if (dist < distToBeat)
		{
			distToBeat = dist;
			bestIndex = i;
		}
	}

	if (distToBeat < 600.0f)
	{
		GameObjectManager::getInstance()->setSelectedObject(gameObjectList[bestIndex]);
	}
}

void Viewport::onLeftMouseUp(const Point deltaPos, const Point currentPt)
{
	
}

void Viewport::onRightMouseDown(const Point deltaPos)
{
}

void Viewport::onRightMouseUp(const Point deltaPos)
{
}
