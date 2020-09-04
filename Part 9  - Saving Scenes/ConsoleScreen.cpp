#include "ConsoleScreen.h"

ConsoleScreen::ConsoleScreen(): AUIScreen("ConsoleScreen")
{
	this->textLog = new ImGuiTextBuffer();
	for (int i = 0; i < 1000; i++)
		this->textLog->appendf("%i The quick brown fox jumps over the lazy dog\n", i);
}

ConsoleScreen::~ConsoleScreen()
{
	delete this->textLog;
}

void ConsoleScreen::drawUI()
{
	ImGui::Begin("Console", 0, ImGuiWindowFlags_NoResize);
	ImGui::SetWindowSize(ImVec2(600, 300));
	if (ImGui::Button("Clear")) { this->textLog->clear(); }
	ImGui::TextUnformatted(this->textLog->begin(), this->textLog->end());
	ImGui::End();
}
