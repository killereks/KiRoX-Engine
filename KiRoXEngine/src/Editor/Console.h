#pragma once

#include <vector>
#include <string>
#include <glm.hpp>

class Console
{
private:
	std::vector<std::string> messages;

	static Console* instance;

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

	void Draw();
	void Clear() {
		messages.clear();
	}
};