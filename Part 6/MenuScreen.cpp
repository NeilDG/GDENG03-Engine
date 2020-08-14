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
			if (ImGui::MenuItem("Create Cube")) { this->OnCreateCubeClicked(); }
			if (ImGui::MenuItem("Create Sphere")) { this->OnCreateSphereClicked(); }
			if (ImGui::MenuItem("Create Plane")) { this->OnCreatePlaneClicked(); }
			if (ImGui::BeginMenu("Light")) {
				if (ImGui::MenuItem("Point Light")) { /* Do stuff */ }
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void MenuScreen::OnCreateCubeClicked()
{
	//initialize vertex for object
	ShaderNames shaderNames;
	void* shaderByteCode = NULL;
	size_t sizeShader = 0;
	GraphicsEngine* graphEngine = GraphicsEngine::getInstance();
	graphEngine->compileVertexShader(shaderNames.BASE_VERTEX_SHADER_NAME.c_str(), "main", &shaderByteCode, &sizeShader);
	GameObjectManager::getInstance()->createObject(GameObjectManager::PrimitiveType::CUBE, shaderByteCode, sizeShader);
}

void MenuScreen::OnCreateSphereClicked()
{
	std::cout << "Creating sphere placeholder. \n";
}

void MenuScreen::OnCreatePlaneClicked()
{
	//initialize vertex for object
	ShaderNames shaderNames;
	void* shaderByteCode = NULL;
	size_t sizeShader = 0;
	GraphicsEngine* graphEngine = GraphicsEngine::getInstance();
	graphEngine->compileVertexShader(shaderNames.BASE_VERTEX_SHADER_NAME.c_str(), "main", &shaderByteCode, &sizeShader);
	GameObjectManager::getInstance()->createObject(GameObjectManager::PrimitiveType::PLANE, shaderByteCode, sizeShader);
}
