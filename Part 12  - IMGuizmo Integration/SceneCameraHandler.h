#pragma once
#include "Camera.h"
#include "vgMath.h"

class SceneCameraHandler
{
public:
	static SceneCameraHandler* getInstance();
	static void initialize();
	static void destroy();

	void update(); //TODO: Game object manager should perform update of all game objects

	Matrix4x4 getSceneCameraViewMatrix() const;
	Matrix4x4 getSceneCameraProjectionMatrix() const;
	Matrix4x4 getSceneCameraLocationMatrix() const;

	Vector3D getCameraRotationXYZ() const;
	Vector3D getCameraLocationXYZ() const;

	//void overrideCameraMatrix(mat4 newMatrix); 
	void setCameraRotation(float x, float y, float z); //from IMGuizmo
	void setCameraLocation(float x, float y, float z); //from IMGuizmo

private:
	SceneCameraHandler();
	~SceneCameraHandler();
	SceneCameraHandler(SceneCameraHandler const&) {};             // copy constructor is private
	SceneCameraHandler& operator=(SceneCameraHandler const&) {};  // assignment operator is private*/
	static SceneCameraHandler* sharedInstance;

	Camera* sceneCamera;

};

