#pragma once
#include <string>

#include <algorithm>
#include <cctype>

class StringTools {
public:
    static std::string FormatComponentName(std::string name) {
        for (size_t i = 1; i < name.length(); ++i) {
            if (isupper(name[i])) {
                name.insert(i, " ");
                ++i;
            }
        }

        size_t pos = name.rfind(" Component");

        if (pos != std::string::npos && pos == name.length() - 10) {
            name.erase(pos);
        }

        return name;
    }


	static std::string ToLowerCase(std::string& input) {
		std::string result = input;
		std::transform(result.begin(), result.end(), result.begin(), ::tolower);
		return result;
	}

	static std::string GetVectorAsString(glm::vec3& vec) {
		return "X: " + std::to_string(vec.x) + ", Y: " + std::to_string(vec.y) + ", Z: " + std::to_string(vec.z);
	}

    static void PrintMatrix(glm::mat4& mat) {
		for (int i = 0; i < 4; ++i) {
			std::cout << mat[i][0] << " " << mat[i][1] << " " << mat[i][2] << " " << mat[i][3] << std::endl;
		}
    }
};