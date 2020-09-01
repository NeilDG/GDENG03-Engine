#include "InspectorScreen.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "GameObjectManager.h"
#include "UIManager.h"
#include "AGameObject.h"
#include "ActionHistory.h"

InspectorScreen::InspectorScreen() : AUIScreen("InspectorScreen")
{
}

InspectorScreen::~InspectorScreen()
{
}

void InspectorScreen::drawUI()
{
	ImGui::Begin("Inspector Window", 0, ImGuiWindowFlags_NoResize);
	ImGui::SetWindowPos(ImVec2(UIManager::WINDOW_WIDTH - 275, 20));
	ImGui::SetWindowSize(ImVec2(250, UIManager::WINDOW_HEIGHT));
	this->selectedObject = GameObjectManager::getInstance()->getSelectedObject();
	if (this->selectedObject != NULL) {
		String name =  this->selectedObject->getName();
		ImGui::Text("Selected Object: %s", name.c_str());
		this->updateTransformDisplays();
		bool enabled = this->selectedObject->isEnabled();
		if (ImGui::Checkbox("Enabled", &enabled)) { this->selectedObject->setEnabled(enabled); }
		ImGui::SameLine();
		if (ImGui::Button("Delete")) { 
			GameObjectManager::getInstance()->deleteObject(this->selectedObject);
			GameObjectManager::getInstance()->setSelectedObject(NULL);
		}
		if (ImGui::InputFloat3("Position", this->positionDisplay, 4)) { this->onTransformUpdate(); }
		if (ImGui::InputFloat3("Rotation", this->rotationDisplay, 4)) { this->onTransformUpdate(); }
		if (ImGui::InputFloat3("Scale", this->scaleDisplay, 4)) { this->onTransformUpdate(); }
		
	}
	else {
		ImGui::Text("No object selected. Select an object first.");
	}
	ImGui::End();
}

void InspectorScreen::updateTransformDisplays()
{
	Vector3D pos = this->selectedObject->getLocalPosition();
	this->positionDisplay[0] = pos.getX();
	this->positionDisplay[1] = pos.getY();
	this->positionDisplay[2] = pos.getZ();

	Vector3D rot = this->selectedObject->getLocalRotation();
	this->rotationDisplay[0] = rot.getX();
	this->rotationDisplay[1] = rot.getY();
	this->rotationDisplay[2] = rot.getZ();

	Vector3D scale = this->selectedObject->getLocalScale();
	this->scaleDisplay[0] = scale.getX();
	this->scaleDisplay[1] = scale.getY();
	this->scaleDisplay[2] = scale.getZ();
}

void InspectorScreen::onTransformUpdate()
{
	if (this->selectedObject != NULL) {
		ActionHistory::getInstance()->recordAction(this->selectedObject);

		this->selectedObject->setPosition(this->positionDisplay[0], this->positionDisplay[1], this->positionDisplay[2]);
		this->selectedObject->setRotation(this->rotationDisplay[0], this->rotationDisplay[1], this->rotationDisplay[2]);
		//this->selectedObject->setEulerAnglesRotation(this->rotationDisplay[0], this->rotationDisplay[1], this->rotationDisplay[2]);
		this->selectedObject->setScale(this->scaleDisplay[0], this->scaleDisplay[1], this->scaleDisplay[2]);
	}
}
