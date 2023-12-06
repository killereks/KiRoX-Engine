#pragma once

#include "glm/glm.hpp"

#include <iomanip>
#include <string>
#include <sstream>

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

};