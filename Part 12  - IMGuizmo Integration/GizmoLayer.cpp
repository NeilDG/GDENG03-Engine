#include "GizmoLayer.h"
#include "imgui.h"
#include "imGuIZMOquat.h"
#include "Matrix4x4.h"
#include "SceneCameraHandler.h"
#include "vgMath.h"
#include "ImGuizmo.h"
#include "UIManager.h"

GizmoLayer::GizmoLayer(): AUIScreen("GizmoLayer")
{
	
}

GizmoLayer::~GizmoLayer()
{
	AUIScreen::~AUIScreen();
}

void GizmoLayer::drawUI()
{
	Matrix4x4 viewMatrix = SceneCameraHandler::getInstance()->getSceneCameraViewMatrix();
	Matrix4x4 projectionMatrix = SceneCameraHandler::getInstance()->getSceneCameraProjectionMatrix();
	Matrix4x4 locationMatrix = SceneCameraHandler::getInstance()->getSceneCameraLocationMatrix();

	ImGui::Begin("Gizmos", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
	float* viewMatrix16 = viewMatrix.getMatrix();
	float* projectionMatrix16 = projectionMatrix.getMatrix();
	float* locationMatrix16 = locationMatrix.getMatrix();
	Matrix4x4 identity; identity.setIdentity();
	float* identityMatrix16 = identity.getMatrix();

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

	SceneCameraHandler::getInstance()->setCameraRotation(rot.x, rot.y, rot.z);
	SceneCameraHandler::getInstance()->setCameraLocation(translation.x, translation.y, translation.z);
	
	ImGui::End();
	
	//TODO: This is just a test for draw grid. Must fix render ordering.
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());
	ImGuizmo::BeginFrame();
    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::DrawGrid(viewMatrix16, projectionMatrix16, identityMatrix16, 100);
	//ImGuizmo::DrawCubes(viewMatrix16, projectionMatrix16, identityMatrix16, 6);
	Manipulate(viewMatrix16, projectionMatrix16, ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, identityMatrix16);

	//ImGui::End();
}



