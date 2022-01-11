#include "SceneCameraHandler.h"
#include "EngineTime.h"

SceneCameraHandler* SceneCameraHandler::sharedInstance = NULL;

SceneCameraHandler* SceneCameraHandler::getInstance()
{
    return sharedInstance;
}

void SceneCameraHandler::initialize()
{
    sharedInstance = new SceneCameraHandler();
}

void SceneCameraHandler::destroy()
{
    delete sharedInstance;
}

void SceneCameraHandler::update()
{
    this->sceneCamera->update(EngineTime::getDeltaTime());
}

Matrix4x4 SceneCameraHandler::getSceneCameraViewMatrix() const
{
    return this->sceneCamera->getViewMatrix();
}

Matrix4x4 SceneCameraHandler::getSceneCameraProjectionMatrix() const
{
    return this->sceneCamera->getProjectionMatrix();
}

Matrix4x4 SceneCameraHandler::getSceneCameraLocationMatrix() const
{
    return this->sceneCamera->getLocalMatrix();
}

SceneCameraHandler::SceneCameraHandler()
{
    this->sceneCamera = new Camera("SceneCamera");
}
SceneCameraHandler::~SceneCameraHandler()
{
    delete this->sceneCamera;
}
