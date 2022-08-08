#include "SceneCameraHandler.h"
#include "EngineTime.h"
#include "MathUtils.h"
#include "UIManager.h"

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
    return this->sceneCamera->getLocalRotationDegrees();
}

Vector3D SceneCameraHandler::getCameraLocationXYZ() const
{
    return this->sceneCamera->getLocalPosition();
}

void SceneCameraHandler::setCameraRotation(float x, float y, float z)
{
    this->sceneCamera->setRotationDegrees(x, y, z);
}

void SceneCameraHandler::setCameraLocation(float x, float y, float z)
{
    this->sceneCamera->setPosition(x, y, z);
}

Vector3D SceneCameraHandler::screenToWorldCoordinates(int screenX, int screenY) const
{
    Matrix4x4 projectionMat = this->getSceneCameraProjectionMatrix();
    Matrix4x4 viewMat = this->getSceneCameraViewMatrix();

    Matrix4x4 newMat;
    newMat = projectionMat.multiplyTo(viewMat);
    newMat.setInverse();

    float x = 4.0f * MathUtils::normalize(screenX * 1.0f, 0.0, UIManager::WINDOW_WIDTH * 1.0f, -1.0f, 1.0f);
    float y = -4.0f * MathUtils::normalize(screenY * 1.0f, 0.0, UIManager::WINDOW_HEIGHT * 1.0f, -1.0f, 1.0f);
    float z = 1.0f;
    float w = 1.0f;

    x = x - viewMat.getTranslation().getX();
    y = y - viewMat.getTranslation().getY();
    //z = z - viewMat.getTranslation().getZ();

    std::cout << "View matrix X: " << viewMat.getTranslation().getX() << " Y: " << viewMat.getTranslation().getY()  << " Z: " <<viewMat.getTranslation().getZ() << std::endl;

    Vector4D in(x, y, z, w);

    Vector4D position = newMat.multiplyTo(in);
    w = 1.0f / position.getW();

    Vector3D worldPos(position.getX() * w, position.getY() * w, position.getZ() * w);

    return worldPos;
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
