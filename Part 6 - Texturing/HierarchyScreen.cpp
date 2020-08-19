#include "HierarchyScreen.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "GameObjectManager.h"
#include "UIManager.h"

HierarchyScreen::HierarchyScreen(): AUIScreen("HierarchyScreen")
{
}

HierarchyScreen::~HierarchyScreen()
{
}

void HierarchyScreen::drawUI()
{
	ImGui::Begin("Scene Outliner");
	ImGui::SetWindowPos(ImVec2(UIManager::WINDOW_WIDTH - 525, 20));
	ImGui::SetWindowSize(ImVec2(250, UIManager::WINDOW_HEIGHT));

	this->updateObjectList();
	ImGui::End();
}

void HierarchyScreen::updateObjectList() const
{
	GameObjectManager::List objectList= GameObjectManager::getInstance()->getAllObjects();
	for (int i = 0; i < objectList.size(); i++) {
		String objectName = objectList[i]->getName();
		if (ImGui::Button(objectName.c_str(), ImVec2(235, 0))) {
			GameObjectManager::getInstance()->setSelectedObject(objectName);
		}
	}
}
