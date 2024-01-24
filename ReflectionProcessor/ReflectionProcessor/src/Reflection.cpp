#include "Reflection.h"

#include <filesystem>
#include <iostream>
#include <fstream>

#include <chrono>

#include "StringHelpers.h"

Reflection::Reflection(const char* targetFilePath) : targetFileLocation(targetFilePath) {}

void Reflection::ReflectFile(const char* defaultPath, const char* filePath)
{
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	std::cout << "Reflecting file " << filePath << "\n";

	std::ifstream file;

	file.open(filePath);

	std::string line;

	PropertyType nextPropType = PropertyType::None;

	bool isReflectionFile = false;

	while (std::getline(file, line)) {
		StringHelpers::trim(line);

		if (nextPropType != PropertyType::None) {
			HandleProperty(line.c_str(), nextPropType);
			nextPropType = PropertyType::None;
		}

		if (line.find("CLASS") == 0) {
			nextPropType = PropertyType::Class;
			isReflectionFile = true;
		}
		else if (line.find("PROPERTY") == 0){
			nextPropType = PropertyType::Property;
		}
		else if (line.find("FUNCTION") == 0) {
			nextPropType = PropertyType::Function;
		}

		// detect if it's an abstract class
		bool hasVirtual = line.find("virtual") != std::string::npos;
		bool hasEqualsZero = line.find("= 0") != std::string::npos || line.find("=0") != std::string::npos;
		if (hasVirtual && hasEqualsZero) {
			GetClass(currentlyParsedClass.c_str()).isAbstract = true;
		}
	}

	if (isReflectionFile) {
		std::string relativePath = StringHelpers::GetRelativePath(defaultPath, filePath);

		GetClass(currentlyParsedClass.c_str()).filePath = filePath;
		GetClass(currentlyParsedClass.c_str()).relativePath = relativePath;

		// if it starts with Components
		bool isComponent = relativePath.find("Components") == 0;

		GetClass(currentlyParsedClass.c_str()).isComponent = isComponent;

		std::cout << "File " << filePath << " has been reflected\n";
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	}

	file.close();
}

void Reflection::HandleProperty(const char* line, PropertyType type)
{
	switch (type) {
		case PropertyType::Class:
		{
			std::string className = StringHelpers::GetBetween(line, "class ", " ");
			GetClass(className.c_str()).name = className;
			currentlyParsedClass = className;

			break;
		}

		case PropertyType::Property:
		{
			std::string propName = StringHelpers::split(line, ' ')[1];
			// remove the semicolon
			propName.pop_back();

			GetClass(currentlyParsedClass.c_str()).properties.push_back(propName);
			break;
		}

		case PropertyType::Function:
		{
			std::string funcName = StringHelpers::split(line, ' ')[1];
			// remove everything up until the first (
			funcName = StringHelpers::GetBetween(funcName, "", "(");

			GetClass(currentlyParsedClass.c_str()).functions.push_back(funcName);

			break;
		}
	}
}

void SetupGLMTypes(std::ofstream& file) {
	file << "registration::class_<glm::vec2>(\"glm::vec2\")\n";
	file << ".constructor<>()\n";
	file << ".property(\"x\", &glm::vec2::x)\n";
	file << ".property(\"y\", &glm::vec2::y);\n";

	file << "registration::class_<glm::vec3>(\"glm::vec3\")\n";
	file << ".constructor<>()\n";
	file << ".property(\"x\", &glm::vec3::x)\n";
	file << ".property(\"y\", &glm::vec3::y)\n";
	file << ".property(\"z\", &glm::vec3::z);\n";

	file << "registration::class_<glm::vec4>(\"glm::vec4\")\n";
	file << ".constructor<>()\n";
	file << ".property(\"x\", &glm::vec4::x)\n";
	file << ".property(\"y\", &glm::vec4::y)\n";
	file << ".property(\"z\", &glm::vec4::z)\n";
	file << ".property(\"w\", &glm::vec4::w);\n";

	file << "registration::class_<glm::quat>(\"glm::quat\")\n";
	file << ".constructor<>()\n";
	file << ".property(\"x\", &glm::quat::x)\n";
	file << ".property(\"y\", &glm::quat::y)\n";
	file << ".property(\"z\", &glm::quat::z)\n";
	file << ".property(\"w\", &glm::quat::w);\n";
}

void SetupGLMStringConversion(std::ofstream& file) {
	file << "inline std::string glm_vec2_to_string(const glm::vec2& vec, bool& ok){\n";
	file << "std::stringstream ss;\n";
	file << "ss << \"[\" << vec.x << \", \" << vec.y << \"]\";\n";
	file << "ok = true;\n";
	file << "return ss.str();\n";
	file << "}\n\n";

	file << "inline std::string glm_vec3_to_string(const glm::vec3& vec, bool& ok){\n";
	file << "std::stringstream ss;\n";
	file << "ss << \"[\" << vec.x << \", \" << vec.y << \", \" << vec.z << \"]\";\n";
	file << "ok = true;\n";
	file << "return ss.str();\n";
	file << "}\n\n";

	file << "inline std::string glm_vec4_to_string(const glm::vec4& vec, bool& ok){\n";
	file << "std::stringstream ss;\n";
	file << "ss << \"[\" << vec.x << \", \" << vec.y << \", \" << vec.z << \", \" << vec.w << \"]\";\n";
	file << "ok = true;\n";
	file << "return ss.str();\n";
	file << "}\n\n";

	file << "inline std::string glm_quat_to_string(const glm::quat& quat, bool& ok){\n";
	file << "std::stringstream ss;\n";
	file << "ss << \"[\" << quat.x << \", \" << quat.y << \", \" << quat.z << \", \" << quat.w << \"]\";\n";
	file << "ok = true;\n";
	file << "return ss.str();\n";
	file << "}\n\n";
}

void Reflection::GenerateFinalFile()
{
	// remove class with empty name
	classes.erase(std::remove_if(classes.begin(), classes.end(), [](ClassData& classData) {
		return classData.name == "";
		}), classes.end());

	std::ofstream file;

	std::string finalFileLocation = std::string(targetFileLocation) + "\\refl.gen.h";

	file.open(finalFileLocation.c_str());

	file << "#pragma once\n\n";

	file << "#include <rttr/registration>\n\n";
	file << "#include <glm/glm.hpp>\n";
	file << "#include <glm/gtc/quaternion.hpp>\n\n";
	file << "#include <string>\n";
	file << "#include <sstream>\n\n";

	for (auto& classInfo : classes) {
		//file << "class " << classInfo.name << ";\n";
		file << "#include \"" << classInfo.relativePath << "\"\n";
	}

	file << "using namespace rttr;\n\n";

	SetupGLMStringConversion(file);

	file << "RTTR_REGISTRATION { \n";

	SetupGLMTypes(file);

	bool useConstructor = true;

	for (auto& classInfo : classes) {
		file << "registration::class_<" << classInfo.name << ">(\"" << classInfo.name << "\")\n";
		if (!classInfo.isAbstract) {
			file << ".constructor<>()\n";
		}
		for (auto& prop : classInfo.properties) {
			file << ".property(\"" << prop << "\", &" << classInfo.name << "::" << prop << ")\n";
		}

		for (auto& func : classInfo.functions) {
			file << ".method(\"" << func << "\", &" << classInfo.name << "::" << func << ")\n";
		}

		file << ";\n\n";
	}

	file << "}\n\n";

	file << "namespace Reflection {\n\n";

	// CONVERTER FUNCTIONS
	for (auto& classInfo : classes) {
		if (!classInfo.isComponent) continue;

		file << "inline " << classInfo.name << "*" << " converter_" << classInfo.name << "(Component* comp, bool& ok){\n";
		file << "ok = true;\n";
		file << "return dynamic_cast<" << classInfo.name << "*>(comp);\n";
		file << "}\n\n";
	}

	// GetType function
	file << "static const rttr::type GetType(const std::string& name){\n";
	
	for (auto& classInfo : classes) {
		if (classInfo.isAbstract) continue;

		file << "if (name == \"" << classInfo.name << "\"){\n";
		file << "return rttr::type::get<" << classInfo.name << "*>();\n";
		file << "}\n";
	}

	file << "std::cout << \"No type found for \" << name << \"\\n\";\n";
	file << "return rttr::type::get<void>();\n";
	file << "}\n\n";

	// COMPONENT CONSTRUCTORS
	file << "static Component* CreateComponent(const std::string& name){\n";
	for (auto& classInfo : classes) {
		if (classInfo.isAbstract) continue;
		if (!classInfo.isComponent) continue;

		file << "if (name == \"" << classInfo.name << "\"){\n";
		file << "return new " << classInfo.name << "();\n";
		file << "}\n";
	}
	file << "throw std::runtime_error(\"No type found for \" + name);\n";
	file << "}\n\n";

	// register types
	file << "static void RegisterTypes(){\n";
	for (auto& classInfo : classes) {
		if (!classInfo.isComponent) continue;
		if (classInfo.isAbstract) continue;

		file << "rttr::type::get<Component*>().register_converter_func(converter_" << classInfo.name << ");\n";
	}

	// glm conversion to string
	file << "rttr::type::get<glm::vec2>().register_converter_func(glm_vec2_to_string);\n";
	file << "rttr::type::get<glm::vec3>().register_converter_func(glm_vec3_to_string);\n";
	file << "rttr::type::get<glm::vec4>().register_converter_func(glm_vec4_to_string);\n";
	file << "rttr::type::get<glm::quat>().register_converter_func(glm_quat_to_string);\n";

	file << "}\n\n";

	// get all components that are not abstract
	file << "static std::vector<rttr::type> GetAllNonAbstractComponents(){\n";
	file << "std::vector<rttr::type> types;\n";
	for (auto& classInfo : classes) {
		if (!classInfo.isComponent) continue;
		if (classInfo.isAbstract) continue;

		file << "types.push_back(rttr::type::get<" << classInfo.name << ">());\n";
	}
	file << "return types;\n";

	file << "}\n\n";

	file << "}\n\n";

	file.close();

	std::cout << "Saved final file to " << finalFileLocation << "\n";
}

ClassData& Reflection::GetClass(const char* name)
{
	for (auto& classData : classes) {
		if (classData.name == name) {
			return classData;
		}
	}

	classes.push_back(ClassData(name));
	return classes.back();
}

void Reflection::ReflectRecursive(const char* folderPath)
{
	for (auto& entry : std::filesystem::recursive_directory_iterator(folderPath)) {
		if (std::filesystem::is_directory(entry)) {
			continue;
		}

		auto extension = entry.path().extension().string();
		if (extension != ".h" && extension != ".hpp") {
			continue;
		}

		ReflectFile(folderPath, entry.path().string().c_str());
	}

	std::cout << "========================\n";
	std::cout << "Reflection results:\n";
	std::cout << "========================\n";

	for (auto& classData : classes) {
		std::cout << "========================\n";
		std::cout << "File: " << classData.filePath << "\n";
		std::cout << "Relative Path: " << classData.relativePath << "\n";
		std::cout << "Class: " << classData.name << "\n";

		std::cout << "Properties:\n";
		for (auto& prop : classData.properties) {
			std::cout << "\t" << prop << "\n";
		}

		std::cout << "Functions:\n";
		for (auto& func : classData.functions) {
			std::cout << "\t" << func << "\n";
		}

		std::cout << "========================\n";
	}

	GenerateFinalFile();
}
