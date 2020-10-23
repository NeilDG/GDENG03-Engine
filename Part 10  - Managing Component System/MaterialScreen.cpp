#include "MaterialScreen.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "TextureManager.h"
#include "Texture.h"
#include "Debug.h"

MaterialScreen::MaterialScreen(): AUIScreen("MaterialScreen")
{
	this->openSceneDialog = new ImGui::FileBrowser();
	this->openSceneDialog->SetTitle("Open Scene");
	this->openSceneDialog->SetTypeFilters({ ".jpg", ".png"});
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

		//test texture
		Texture* tex = (Texture*)TextureManager::getInstance()->createTextureFromFile(L"D:/Users/delgallegon/Documents/GithubProjects/GDENG2-Engine/Part 10  - Managing Component System/Assets/Textures/no_texture.png");
		ID3D11ShaderResourceView* shaderResource = tex->getShaderResource();
		ImGui::SetCursorPosX(50);
		ImGui::Image((void*)shaderResource, ImVec2(300, 300));

		if(ImGui::Button("Load Texture", ImVec2(WINDOW_WIDTH - 15, 25)))
		{
			this->openSceneDialog->Open();
		}

		this->openSceneDialog->Display();
		if (this->openSceneDialog->HasSelected())
		{
			Debug::Log("Selected: " +this->openSceneDialog->GetSelected().string());
			this->openSceneDialog->ClearSelected();
			this->openSceneDialog->Close();
		}
		ImGui::End();
	}
	
}
