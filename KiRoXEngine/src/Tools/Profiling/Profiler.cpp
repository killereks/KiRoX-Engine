#include "Profiler.h"
#include <imgui.h>
#include <Engine.h>

#include "imgui_stdlib.h"

Profiler* Profiler::instance = nullptr;

void Profiler::BeginFrame() {
	if (isPaused) return;

	scopeStack = std::stack<std::string>();
	profileData.clear();

	beginFrameTime = std::chrono::high_resolution_clock::now();
}

void Profiler::Push(const char* name) {
	if (GetInstance()->isPaused) return;

	auto now = std::chrono::high_resolution_clock::now();

	std::string fullName = std::string(name);
	GetInstance()->scopeStack.push(fullName);

	ProfileData data;
	data.name = const_cast<char*>(fullName.c_str());
	data.startTime = now;
	data.depth = GetInstance()->scopeStack.size();

	GetInstance()->profileData.emplace(fullName, data);

	auto test = GetInstance()->profileData.find(fullName);
}

void Profiler::Pop(const char* name) {
	if (GetInstance()->isPaused) return;

	assert(!GetInstance()->scopeStack.empty());

	auto now = std::chrono::high_resolution_clock::now();
	std::string fullName = std::string(name);
	GetInstance()->scopeStack.pop();

	ProfileData& data = GetInstance()->profileData[fullName];
	data.endTime = now;
}

void Profiler::EndFrame() {
	assert(scopeStack.empty());

	ImGui::Begin("Profiler");

	if (isPaused) {
		if (ImGui::Button("Resume")) {
			isPaused = false;
		}
	}
	else {
		if (ImGui::Button("Pause")) {
			isPaused = true;
		}
		endFrameTime = std::chrono::high_resolution_clock::now();
	}
	ImGui::SameLine();
	ImGui::Checkbox("Use Time Threshold", &useTimeThreshold);
	if (useTimeThreshold) {
		ImGui::SameLine();
		ImGui::DragFloat("Threshold (ms)", &timeThreshold, 0.01f, 0.0f);
		timeThreshold = std::max(0.0f, timeThreshold);
	}
	ImGui::InputText("Filter...", &currentFilter);

	float totalFrameTime = std::chrono::duration_cast<std::chrono::microseconds>(endFrameTime - beginFrameTime).count() / 1000.0f;

	ImGui::Text("Total Frame Time: %.2fms", totalFrameTime);

	for (auto& [name, data] : profileData) {
		std::string nameLower = name;
		std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
		std::string filterLower = currentFilter;
		std::transform(filterLower.begin(), filterLower.end(), filterLower.begin(), ::tolower);
		if (currentFilter.size() > 0 && nameLower.find(filterLower) == std::string::npos) {
			continue;
		}
		if (useTimeThreshold && data.GetDuration() < timeThreshold) {
			continue;
		}

		int indentation = data.depth * 20;

		ImGui::Indent(data.depth * 20);
		ImGui::Text("%s: %.2fms", name.c_str(), data.GetDuration());
		ImGui::Unindent(data.depth * 20);
	}

	ImGui::End();
}