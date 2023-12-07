#pragma once

#include "glm/glm.hpp"

#include <iomanip>
#include <string>
#include <sstream>

#include "imgui.h"
#include "imgui_internal.h"

class Mathf
{
public:
	inline static float Lerp(float a, float b, float t)
	{
		t = glm::clamp(t, 0.0f, 1.0f);
		return LerpUnclamped(a, b, t);
	}

	inline static float LerpUnclamped(float a, float b, float t)
	{
		return a + (b - a) * t;
	}

	template<class T>
	inline static std::string FormatWithCommas(T value)
	{
		std::stringstream ss;
		ss.imbue(std::locale(""));
		ss << std::fixed << value;
		return ss.str();
	}

	inline static void RenderTextWithEllipsis(const std::string& text, float maxWidth) {
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		const ImVec2 textSize = ImGui::CalcTextSize(text.c_str());

		if (textSize.x > maxWidth) {
			ImVec2 pos_min = ImGui::GetCursorScreenPos();
			ImVec2 pos_max = ImVec2(pos_min.x + maxWidth, pos_min.y + textSize.y);

			float clip_max_x = pos_max.x;
			float ellipsis_max_x = pos_max.x;

			ImGui::RenderTextEllipsis(draw_list, pos_min, pos_max, clip_max_x, ellipsis_max_x, text.c_str(), NULL, &textSize);
		}
		else {
			ImGui::Text("%s", text.c_str());
		}

	}
};