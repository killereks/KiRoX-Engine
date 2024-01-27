#pragma once

#include <vector>
#include <string>
#include <glm.hpp>
#include "imgui.h"
#include <sstream>
#include <iomanip>
#include <fstream>

class Console
{
private:
	std::vector<std::string> messages;
	std::vector<ImVec4> colors;

	static Console* instance;

	bool scrollToBottom;

	int capacity = 2000;

	std::string outputPath = "";

	public:
	Console();
	~Console();

	static void SetOutputPath(std::string path) {
		Console* console = GetInstance();
		console->outputPath = path;

		Write("Console output path set to " + path);
	}

	static Console* GetInstance() {
		if (instance == nullptr) {
			instance = new Console();
		}
		return instance;
	}

	static void Write(std::string message)
	{
		Write(message, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
	}

	static void Write(std::string message, ImVec4 color)
	{
		auto t = std::time(nullptr);
		std::tm tm;
		localtime_s(&tm, &t);

		// format to hh:mm:ss
		std::ostringstream oss;
		oss << std::put_time(&tm, "%H:%M:%S");

		std::string time = oss.str();

		Console* console = GetInstance();

		std::string formattedMessage = "[" + time + "] " + message;

		console->messages.push_back(formattedMessage.c_str());
		console->colors.push_back(color);

		if (console->messages.size() > console->capacity)
		{
			int overflow = (int)console->messages.size() - console->capacity;
			console->messages.erase(console->messages.begin(), console->messages.begin() + overflow);
			console->colors.erase(console->colors.begin(), console->colors.begin() + overflow);
		}

		if (console->outputPath != "") {
			std::ofstream file;
			file.open(console->outputPath.c_str(), std::ios::app);
			file << formattedMessage << "\n";
			file.close();
		}
	}

	void Draw();
	void Clear() {
		messages.clear();
	}
};