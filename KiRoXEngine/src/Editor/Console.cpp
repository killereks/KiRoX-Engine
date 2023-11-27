#include "Console.h"

#include <iomanip>
#include <ctime>
#include <sstream>

#include "imgui.h"

Console* Console::instance = nullptr;

Console::Console()
{

}

Console::~Console()
{
	delete instance;
}

void Console::Draw()
{
	ImGui::Begin("Console");

	ImGui::BeginChild("Settings", ImVec2(0, 30));

	if (ImGui::Button("Clear")) {
		Clear();
	}

	ImGui::SameLine();
	ImGui::Text("%i logs...", messages.size());

	ImGui::SameLine();
	ImGui::Checkbox("Scroll to bottom", &scrollToBottom);

	ImGui::SameLine();
	if (ImGui::Button("Debug Text")) {
		Write("Hello, this is a debug text!", ImVec4(1.0, 1.0, 1.0, 1.0));
	}

	ImGui::EndChild();

	ImGui::BeginChild("ScrollRegion");
	for (int i = 0; i < messages.size(); i++)
	{
		ImGui::TextColored(colors[i], messages[i].c_str());
		ImGui::Separator();
	}

	if (scrollToBottom) {
		ImGui::SetScrollHereY(0.999f);
	}

	ImGui::EndChild();

	ImGui::End();
}
