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

void Console::Write(std::string message)
{
	auto t = std::time(nullptr);
	std::tm tm;
	localtime_s(&tm, &t);

	// format to hh:mm:ss
	std::ostringstream oss;
	oss << std::put_time(&tm, "%H:%M:%S");

	std::string time = oss.str();

	GetInstance()->messages.push_back("[" + time + "] " + message);
}

void Console::Draw()
{
	ImGui::Begin("Console");

	ImGui::BeginChild("Settings", ImVec2(0, 25));

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
	if (ImGui::Button("Clear")) {
		Clear();
	}
	ImGui::PopStyleColor();

	ImGui::SameLine();

	ImGui::Text("%i logs...", messages.size());

	ImGui::EndChild();


	ImGui::BeginChild("ScrollRegion");
	for (int i = 0; i < messages.size(); i++)
	{
		if (i % 2 == 0)
			ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), messages[i].c_str());
		else {
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), messages[i].c_str());
		}
	}
	ImGui::EndChild();

	ImGui::End();
}
