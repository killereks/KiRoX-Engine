#pragma once

#include <vector>
#include <string>

struct ClassData {
	std::string name;
	std::vector<std::string> properties;
	std::vector<std::string> functions;

	bool isAbstract = false;

	bool isComponent = false;

	std::string filePath;
	std::string relativePath;
};

class Reflection
{
	enum class PropertyType {
		None = 0,
		Class,
		Property,
		Function
	};

	std::string currentlyParsedClass;

	const char* targetFileLocation;

	std::vector<ClassData> classes;

	void ReflectFile(const char* defaultPath, const char* filePath);
	void HandleProperty(const char* line, PropertyType type);

	void GenerateFinalFile();

	ClassData& GetClass(const char* name);

public:
	Reflection(const char* targetFilePath);
	void ReflectRecursive(const char* folderPath);

};

