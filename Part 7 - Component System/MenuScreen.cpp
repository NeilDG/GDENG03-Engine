#include "MenuScreen.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <iostream>
#include "GameObjectManager.h"
#include "GraphicsEngine.h"
#include "VertexShader.h"
#include "ShaderLibrary.h"

MenuScreen::MenuScreen(): AUIScreen("MenuScreen")
{

}

MenuScreen::~MenuScreen()
{
}

void MenuScreen::drawUI()
{
	
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
			if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
			if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) { /* Do stuff */ }
			if (ImGui::MenuItem("Exit Editor", "Ctrl+W")) {/*Do something */ }
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Game Object")) {
			if (ImGui::MenuItem("Create Sphere")) { this->OnCreateSphereClicked(); }
			if (ImGui::MenuItem("Create Cube")) { this->OnCreateCubeClicked(); }
			if (ImGui::MenuItem("Create Textured Cube")) { this->OnCreateTexturedCubeClicked(); }
			if (ImGui::MenuItem("Create Placeholder Physics Cube")) { this->OnPhysicsCubeClicked(); }
			if (ImGui::MenuItem("Create Plane")) { this->OnCreatePlaneClicked(); }
			if (ImGui::MenuItem("Create Placeholder Physics Plane")) { this->OnPhysicsPlaneClicked(); }
			if (ImGui::BeginMenu("Light")) {
				if (ImGui::MenuItem("Point Light")) { /* Do stuff */ }
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Components")) {
			if (ImGui::MenuItem("Rigid Body")) { this->OnRigidBodyComponentClicked(); }
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void MenuScreen::OnCreateCubeClicked()
{
	//initialize vertex for object
	GameObjectManager::getInstance()->createObject(GameObjectManager::PrimitiveType::CUBE);
}

void MenuScreen::OnCreateTexturedCubeClicked()
{
	GameObjectManager::getInstance()->createObject(GameObjectManager::PrimitiveType::TEXTURED_CUBE);
}

void MenuScreen::OnCreateSphereClicked()
{
	std::cout << "Creating sphere placeholder. \n";
}

void MenuScreen::OnCreatePlaneClicked()
{
	//initialize vertex for object
	GameObjectManager::getInstance()->createObject(GameObjectManager::PrimitiveType::PLANE);
}

void MenuScreen::OnRigidBodyComponentClicked()
{
	std::cout << "Creating rigid body placeholder. \n";
}

void MenuScreen::OnPhysicsCubeClicked()
{
	for (int i = 0; i < 20; i++) {
		GameObjectManager::getInstance()->createObject(GameObjectManager::PrimitiveType::PHYSICS_CUBE);
	}
}

void MenuScreen::OnPhysicsPlaneClicked()
{
	GameObjectManager::getInstance()->createObject(GameObjectManager::PrimitiveType::PHYSICS_PLANE);
}
