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

Vector3D SceneCameraHandler::getCameraRotationXYZ() const
{
    return this->sceneCamera->getLocalRotation();
}

Vector3D SceneCameraHandler::getCameraLocationXYZ() const
{
    return this->sceneCamera->getLocalPosition();
}

void SceneCameraHandler::setCameraRotation(float x, float y, float z)
{
    this->sceneCamera->setRotation(x, y, z);
}

void SceneCameraHandler::setCameraLocation(float x, float y, float z)
{
    this->sceneCamera->setPosition(x, y, z);
}

/*void SceneCameraHandler::overrideCameraMatrix(mat4 newMatrix)
{
    Matrix4x4 matrixCompatible;
    float mat16[16] = {newMatrix.m00, newMatrix.m01, newMatrix.m02, newMatrix.m03,
        newMatrix.m10, newMatrix.m11, newMatrix.m12, newMatrix.m13,
        newMatrix.m20, newMatrix.m21, newMatrix.m22, newMatrix.m23,
        newMatrix.m30, newMatrix.m31, newMatrix.m32, newMatrix.m33
    };
    matrixCompatible.setMatrix(mat16);

    this->sceneCamera->recomputeMatrix(mat16);
}*/

SceneCameraHandler::SceneCameraHandler()
{
    this->sceneCamera = new Camera("SceneCamera");
}
SceneCameraHandler::~SceneCameraHandler()
{
    delete this->sceneCamera;
}
