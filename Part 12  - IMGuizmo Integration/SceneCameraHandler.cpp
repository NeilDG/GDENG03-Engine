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

    float x =1.0f * MathUtils::normalize(screenX * 1.0f, 0.0, UIManager::WINDOW_WIDTH * 1.0f, -1.0f, 1.0f);
    float y = -1.0f * MathUtils::normalize(screenY * 1.0f, 0.0, UIManager::WINDOW_HEIGHT * 1.0f, -1.0f, 1.0f);
    float z = 1.0f;
    float w = 1.0f;

    //x = x - viewMat.getTranslation().getX();
    //y = y - viewMat.getTranslation().getY();
    //z = z - viewMat.getTranslation().getZ();

    std::cout << "View matrix X: " << viewMat.getTranslation().getX() << " Y: " << viewMat.getTranslation().getY()  << " Z: " <<viewMat.getTranslation().getZ() << std::endl;

    Vector4D in(x, y, z, w);

    Vector4D position = newMat.multiplyTo(in);
    w = 1.0f / position.getW();

    Vector3D worldPos(position.getX() * w, position.getY() * w, position.getZ() * w);


    //return worldPos;

    Matrix4x4 revisedMat; revisedMat.setTranslation(worldPos);
    Matrix4x4 cameraLocMat = this->getSceneCameraLocationMatrix();
    //revisedMat = revisedMat.multiplyTo(cameraLocMat);
    revisedMat = revisedMat.multiplyTo(viewMat);
    revisedMat = revisedMat.multiplyTo(projectionMat);
    return revisedMat.getTranslation();
}

Vector2D SceneCameraHandler::worldToScreenCoordinates(float x, float y, float z) const
{
    std::cout << "World X: " << x << " " << y << " " << z << std::endl;

    Matrix4x4 projection = this->getSceneCameraProjectionMatrix();
    Matrix4x4 view = this->getSceneCameraViewMatrix();
    Matrix4x4 loc = this->getSceneCameraLocationMatrix();
    Matrix4x4 viewSpace = loc.multiplyTo(view);
    Matrix4x4 projSpace = viewSpace.multiplyTo(projection);

    float* projectionMat = projSpace.getMatrix16();

    float clipX = x * projectionMat[0] + y * projectionMat[1] + z * projectionMat[2] + projectionMat[3];
    float clipY = x * projectionMat[4] + y * projectionMat[5] + z * projectionMat[6] + projectionMat[7];
    float clipZ = x * projectionMat[8] + y * projectionMat[9] + z * projectionMat[10] + projectionMat[11];
    float clipW = x * projectionMat[12] + y * projectionMat[13] + z * projectionMat[14] + projectionMat[15];

	//normalize device coordinates
    float ndcX = clipX / clipW;
    float ndcY = clipY / clipW;
    float ndcZ = clipZ / clipW;

    float screenX = (UIManager::WINDOW_WIDTH / 2.0f * ndcX) + (ndcX + UIManager::WINDOW_WIDTH / 2.0f);
    float screenY = -(UIManager::WINDOW_HEIGHT / 2.0f * ndcY) + (ndcY + UIManager::WINDOW_HEIGHT / 2.0f);

    return Vector2D(screenX, screenY);
}

Vector2D SceneCameraHandler::worldToScreenCoordinates(Vector3D pos) const
{
    return this->worldToScreenCoordinates(pos.getX(), pos.getY(), pos.getZ());
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
