#pragma once

#include <vector>
#include <string>
#include <glm.hpp>

class Console
{
private:
	std::vector<std::string> messages;

	static Console* instance;

	bool scrollToBottom;

	int capacity = 250;

	public:
	Console();
	~Console();

	static Console* GetInstance() {
		if (instance == nullptr) {
			instance = new Console();
		}
		return instance;
	}

	static void Write(std::string message);
	static void Write(glm::vec2 vector) {
		Write(std::to_string(vector.x) + ", " + std::to_string(vector.y));
	}

	static void Write(glm::vec3 vector) {
		Write(std::to_string(vector.x) + ", " + std::to_string(vector.y)+", "+std::to_string(vector.z));
	}

	static void Write(glm::vec4 vector) {
		Write(std::to_string(vector.x) + ", " + std::to_string(vector.y) + ", " + std::to_string(vector.z)+", "+std::to_string(vector.w));
	}

	void Draw();
	void Clear() {
		messages.clear();
	}
};