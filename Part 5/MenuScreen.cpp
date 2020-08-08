#include "MenuScreen.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

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
			if (ImGui::MenuItem("Create Cube")) { /* Do stuff */ }
			if (ImGui::MenuItem("Create Capsule")) { /* Do stuff */ }
			if (ImGui::MenuItem("Create Plane")) { /* Do stuff */ }
			if (ImGui::BeginMenu("Light")) {
				if (ImGui::MenuItem("Point Light")) { /* Do stuff */ }
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}
