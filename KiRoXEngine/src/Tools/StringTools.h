#pragma once
#include <string>

#include <algorithm>
#include <cctype>

class StringTools {
public:
	static std::string FormatComponentName(std::string name) {
		size_t pos = name.rfind("Component");

		if (pos != std::string::npos && pos == name.length() - 9) {
			name.erase(pos);
		}

		return name;
	}

	static std::string ToLowerCase(std::string& input) {
		std::string result = input;
		std::transform(result.begin(), result.end(), result.begin(), ::tolower);
		return result;
	}
};