#include "GizmoLayer.h"
#include "imgui.h"
#include "imGuIZMOquat.h"
#include "Matrix4x4.h"
#include "SceneCameraHandler.h"
#include "vgMath.h"
#include "ImGuizmo.h"
#include "UIManager.h"
#include "GameObjectManager.h"

GizmoLayer* GizmoLayer::sharedInstance = nullptr;

GizmoLayer* GizmoLayer::getInstance()
{
	return sharedInstance;
}

void GizmoLayer::initialize()
{
	sharedInstance = new GizmoLayer();
}

void GizmoLayer::destroy()
{
	delete sharedInstance;
}


GizmoLayer::GizmoLayer() {

}

GizmoLayer::~GizmoLayer()
{

}

void GizmoLayer::draw()
{	
	
	ImGui::Begin("Gizmos", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
	
	Vector3D cameraRot = SceneCameraHandler::getInstance()->getCameraRotationXYZ();
	quat rot(1.0f, cameraRot.getX(), cameraRot.getY(), cameraRot.getZ());

	Vector3D cameraLoc = SceneCameraHandler::getInstance()->getCameraLocationXYZ();
	vec3 translation(cameraLoc.getX(), cameraLoc.getY(), cameraLoc.getZ());
	if(ImGui::gizmo3D("Camera Rot", rot, 100, imguiGizmo::mode3Axes | imguiGizmo::cubeAtOrigin))
	{
		SceneCameraHandler::getInstance()->setCameraRotation(rot.x, rot.y, rot.z);
	}
	ImGui::SameLine();
	if(ImGui::gizmo3D("Camera Translate", translation, 100, imguiGizmo::modePanDolly | imguiGizmo::cubeAtOrigin))
	{
		SceneCameraHandler::getInstance()->setCameraLocation(translation.x, translation.y, translation.z);
	}

	ImGui::Checkbox("Show Grid", &this->gridShow);

	SceneCameraHandler::getInstance()->setCameraRotation(rot.x, rot.y, rot.z);
	SceneCameraHandler::getInstance()->setCameraLocation(translation.x, translation.y, translation.z);
	
	if (ImGui::RadioButton("Translate", this->translate))
	{
		this->translate = true;
		this->rotate = false;
		this->scale = false;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", this->rotate))
	{
		this->translate = false;
		this->rotate = true;
		this->scale = false;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", this->scale))
	{
		this->translate = false;
		this->rotate = false;
		this->scale = true;
	}
	ImGui::End();
	
	ImGuizmo::BeginFrame();
    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	Matrix4x4 viewMatrix = SceneCameraHandler::getInstance()->getSceneCameraViewMatrix();
	Matrix4x4 projectionMatrix = SceneCameraHandler::getInstance()->getSceneCameraProjectionMatrix();
	Matrix4x4 locationMatrix = SceneCameraHandler::getInstance()->getSceneCameraLocationMatrix();

	float* viewMatrix16 = viewMatrix.getMatrix();
	float* projectionMatrix16 = projectionMatrix.getMatrix();
	float* locationMatrix16 = locationMatrix.getMatrix();
	
	if(this->gridShow)
	{
		Matrix4x4 identity; identity.setIdentity();
		float* identityMatrix16 = identity.getMatrix();
		ImGuizmo::DrawGrid(viewMatrix16, projectionMatrix16, identityMatrix16, 100); //draw grid
	}
	
	
	this->selectedObject = GameObjectManager::getInstance()->getSelectedObject();
	if(this->selectedObject != nullptr)
	{
		float* locationMatrix = this->selectedObject->getLocalMatrix().getMatrix();
		
		if(this->translate) Manipulate(viewMatrix16, projectionMatrix16, ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, locationMatrix);
		if(this->rotate) Manipulate(viewMatrix16, projectionMatrix16, ImGuizmo::ROTATE, ImGuizmo::LOCAL, locationMatrix);
		if(this->scale) Manipulate(viewMatrix16, projectionMatrix16, ImGuizmo::SCALE, ImGuizmo::LOCAL, locationMatrix);

		if(this->translate || this->rotate || this->scale)
		{
			float translate[3] = { 0.0f, 0.0f, 0.0f };
			float rotate[3] = { 0.0f, 0.0f, 0.0f };
			float scale[3] = { 0.0f, 0.0f, 0.0f };
			ImGuizmo::DecomposeMatrixToComponents(locationMatrix, translate, rotate, scale);
			//this->selectedObject->setNewMatrix(locationMatrix);

			this->selectedObject->setPosition(translate[0], translate[1], translate[2]);
			this->selectedObject->setRotationDegrees(rotate[0], rotate[1], rotate[2]);
			this->selectedObject->setScale(scale[0], scale[1], scale[2]);
		}
		
	}
	
}

void GizmoLayer::enableGizmo(AGameObject* selectedObject)
{
	this->selectedObject = selectedObject;
}



