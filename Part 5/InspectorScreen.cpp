#include "InspectorScreen.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "GameObjectManager.h"

InspectorScreen::InspectorScreen() : AUIScreen("InspectorScreen")
{
}

InspectorScreen::~InspectorScreen()
{
}

void InspectorScreen::drawUI()
{
	ImGui::Begin("Inspector Window"); 
	ImGui::SetWindowSize(ImVec2(400, 150));
	if (!this->objectNameString.empty()) {
		const char* forPrint = this->objectNameString.c_str();
		ImGui::Text("Selected Object: %s", forPrint);
	}
	
	if (ImGui::InputFloat3("Position", this->positionDisplay, 4)) { this->onTransformUpdate(); }
	if (ImGui::InputFloat3("Rotation", this->rotationDisplay, 4)) { this->onTransformUpdate(); }
	if (ImGui::InputFloat3("Scale", this->scaleDisplay, 4)) { this->onTransformUpdate(); }
	ImGui::End();
}

void InspectorScreen::onTransformUpdate()
{
	AGameObject* firstObject = GameObjectManager::getInstance()->findObjectByName("Cube");
	firstObject->setPosition(Vector3D(this->positionDisplay[0], this->positionDisplay[1], this->positionDisplay[2]));
	firstObject->setRotation(Vector3D(this->rotationDisplay[0], this->rotationDisplay[1], this->rotationDisplay[2]));
	firstObject->setScale(Vector3D(this->scaleDisplay[0], this->scaleDisplay[1], this->scaleDisplay[2]));

	this->objectNameString = firstObject->getName();
	std::cout << "Name is: " << this->objectNameString << "\n";
}
