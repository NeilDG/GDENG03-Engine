#include "ActionScreen.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ActionHistory.h"
#include "GameObjectManager.h"

ActionScreen::ActionScreen(): AUIScreen("ActionScreen")
{

}

ActionScreen::~ActionScreen()
{
}

void ActionScreen::drawUI()
{
	ImGui::Begin("Actions", 0, ImGuiWindowFlags_NoResize);
	ImGui::SetWindowSize(ImVec2(205, 75));
	ImGui::SameLine();
	if (ImGui::Button("Undo")) {
		if (ActionHistory::getInstance()->hasRemainingUndoActions()) {
			GameObjectManager::getInstance()->applyEditorAction(ActionHistory::getInstance()->undoAction());
		}
		
	}
	ImGui::SameLine();
	if (ImGui::Button("Redo")) {
		if (ActionHistory::getInstance()->hasRemainingRedoActions()) {
			GameObjectManager::getInstance()->applyEditorAction(ActionHistory::getInstance()->redoAction());
		}
	}
	ImGui::End();
}
