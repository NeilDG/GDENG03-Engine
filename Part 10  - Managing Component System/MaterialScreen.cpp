#include "MaterialScreen.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "TextureManager.h"
#include "Texture.h"
#include "Debug.h"
#include "InspectorScreen.h"

void MaterialScreen::linkInspectorScreen(InspectorScreen* inspectorScreen)
{
	this->inspectorScreen = inspectorScreen;
}

MaterialScreen::MaterialScreen(): AUIScreen("MaterialScreen")
{
	this->openSceneDialog = new ImGui::FileBrowser();
	this->openSceneDialog->SetTitle("Open Scene");
	this->openSceneDialog->SetTypeFilters({ ".jpg", ".png"});

	const wchar_t* texturePath = L"D:/Users/delgallegon/Documents/GithubProjects/GDENG2-Engine/Part 10  - Managing Component System/Assets/Textures/no_texture.png";
	this->displayTex = static_cast<Texture*>(TextureManager::getInstance()->createTextureFromFile(texturePath));
}

MaterialScreen::~MaterialScreen()
{
	
}

void MaterialScreen::drawUI()
{
	if(this->enabled)
	{
		int WINDOW_WIDTH = 400;
		int WINDOW_HEIGHT = 500;
		
		ImGui::Begin("Materials Editor", 0, ImGuiWindowFlags_NoResize);
		ImGui::SetWindowSize(ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT));

		ImGui::SetCursorPosX(50);
		ImGui::Image((void*)this->displayTex->getShaderResource(), ImVec2(300, 300));

		if(ImGui::Button("Load Texture", ImVec2(WINDOW_WIDTH - 15, 25)))
		{
			this->openSceneDialog->Open();
		}

		if (ImGui::Button("Apply", ImVec2(WINDOW_WIDTH - 15, 25)))
		{
			this->SetEnabled(false);
			this->inspectorScreen->SendResult();
			this->inspectorScreen = nullptr;
		}

		this->openSceneDialog->Display();
		if (this->openSceneDialog->HasSelected())
		{
			Debug::Log("Selected: " +this->openSceneDialog->GetSelected().string());
			std::cout << "\n";

			//convert to wchar format
			String textureString = this->openSceneDialog->GetSelected().string();
			std::wstring widestr = std::wstring(textureString.begin(), textureString.end());
			const wchar_t* texturePath = widestr.c_str();
			
			delete this->displayTex;
			this->displayTex = static_cast<Texture*>(TextureManager::getInstance()->createTextureFromFile(texturePath));
			
			this->openSceneDialog->ClearSelected();
			this->openSceneDialog->Close();
		}
		ImGui::End();
	}
	
}
