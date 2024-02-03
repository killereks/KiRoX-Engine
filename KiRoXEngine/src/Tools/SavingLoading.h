#pragma once

#include <Tools/Serialization.h>
#include <rttr/registration.h>
#include <yaml-cpp/yaml.h>

#include <Tools/ObjectPtr.h>

#include <fstream>

#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Color.h"

namespace SavingLoading {

	inline std::string SaveYAML(rttr::variant& var) {
		YAML::Emitter out;
		out << YAML::BeginMap;

		const rttr::type& type = var.get_type();

		for (auto prop : type.get_properties()) {
			std::string name = prop.get_name();

			std::cout << "Found property: " << name << " on object: " << type.get_name().c_str() << "\n";

			out << YAML::Key << name;

			if (prop.get_value(var).get_type() == rttr::type::get<glm::vec2>()) {
				glm::vec2 value = prop.get_value(var).get_value<glm::vec2>();
				out << YAML::Value << value;
			}
			else if (prop.get_value(var).get_type() == rttr::type::get<glm::vec3>()) {
				glm::vec3 value = prop.get_value(var).get_value<glm::vec3>();
				out << YAML::Value << value;
			}
			else if (prop.get_value(var).get_type() == rttr::type::get<glm::vec4>()) {
				glm::vec4 value = prop.get_value(var).get_value<glm::vec4>();
				out << YAML::Value << value;
			}
			else if (prop.get_value(var).get_type() == rttr::type::get<glm::quat>()) {
				glm::quat value = prop.get_value(var).get_value<glm::quat>();
				out << YAML::Value << value;
			}
			else if (prop.get_value(var).get_type() == rttr::type::get<std::string>()) {
				std::string value = prop.get_value(var).get_value<std::string>();
				out << YAML::Value << value;
			}
			else if (prop.get_value(var).get_type() == rttr::type::get<ObjectPtr*>()) {
				std::string value = prop.get_value(var).get_value<ObjectPtr*>()->GetUUID();
				out << YAML::Value << value;
			}
			else if (prop.get_value(var).get_type() == rttr::type::get<Color>()) {
				Color value = prop.get_value(var).get_value<Color>();

				out << YAML::Value << value.GetAsVector();
			}
			else {
				out << YAML::Value << prop.get_value(var).to_string();
			}
		}

		out << YAML::EndMap;

		return out.c_str();
	}

	inline void LoadYAMLObject(const std::string& path, rttr::variant& obj) {
		YAML::Node data = YAML::LoadFile(path);

		const rttr::type& type = obj.get_type();

		for (auto prop : type.get_properties()) {
			std::string name = prop.get_name();

			YAML::Node propData = data[name];
			if (!propData) continue;

			if (prop.get_value(obj).get_type() == rttr::type::get<glm::vec2>()) {
				glm::vec2 value = data[name].as<glm::vec2>();
				prop.set_value(obj, value);
			}
			else if (prop.get_value(obj).get_type() == rttr::type::get<glm::vec3>()) {
				glm::vec3 value = data[name].as<glm::vec3>();
				prop.set_value(obj, value);
			}
			else if (prop.get_value(obj).get_type() == rttr::type::get<glm::vec4>()) {
				glm::vec4 value = data[name].as<glm::vec4>();
				prop.set_value(obj, value);
			}
			else if (prop.get_value(obj).get_type() == rttr::type::get<glm::quat>()) {
				glm::quat value = data[name].as<glm::quat>();
				prop.set_value(obj, value);
			}
			else if (prop.get_value(obj).get_type() == rttr::type::get<std::string>()) {
				std::string value = data[name].as<std::string>();
				prop.set_value(obj, value);
			}
			else if (prop.get_value(obj).get_type() == rttr::type::get<int>()) {
				int value = data[name].as<int>();
				prop.set_value(obj, value);
			}
			else if (prop.get_value(obj).get_type() == rttr::type::get<float>()) {
				float value = data[name].as<float>();
				prop.set_value(obj, value);
			}
			else if (prop.get_value(obj).get_type() == rttr::type::get<bool>()) {
				bool value = data[name].as<bool>();
				prop.set_value(obj, value);
			}
			else if (prop.get_value(obj).get_type() == rttr::type::get<ObjectPtr*>()) {
				ObjectPtr* value = prop.get_value(obj).get_value<ObjectPtr*>();
				std::string uuid = data[name].as<std::string>();
				value->SetUUID(uuid);
			}
			else if (prop.get_value(obj).get_type() == rttr::type::get<Color>()) {
				glm::vec4 value = data[name].as<glm::vec4>();
				Color color = Color(value.x, value.y, value.z, value.w);
				prop.set_value(obj, color);
			}
			else {
				std::cout << "Could not load property: " << name << "on object: " << type.get_name().c_str() << "\n";
			}
		}
	}

	inline void SaveYAMLFile(const std::string& filePath, rttr::variant& var) {
		std::string data = SaveYAML(var);

		std::ofstream file(filePath);
		file << data;
		file.close();
	}

	template<typename T>
	T LoadYAMLFile(const std::string& filePath) {
		std::ifstream file(filePath);
		std::string bufferData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

		return LoadYAML<T>(bufferData);
	}

	template<typename T>
	T LoadYAML(const std::string bufferData) {
		YAML::Node data = YAML::Load(bufferData);

		T obj;

		const rttr::type& type = rttr::type::get<T>();

		LoadYAMLObject(obj);

		return obj;
	}

}