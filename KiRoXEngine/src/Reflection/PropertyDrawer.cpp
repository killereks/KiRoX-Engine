#include "PropertyDrawer.h"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <glm/glm.hpp>
#include <iostream>

void PropertyDrawer::DrawProperty(rttr::property& prop, rttr::variant& objInstance)
{
	// TODO: Add array support
	// TODO: Add enum support
	DrawSingleProperty(prop, objInstance);
}

void PropertyDrawer::DrawSingleProperty(rttr::property& prop, rttr::variant& objInstance)
{
	if (prop.get_type() == rttr::type::get<int>()) {
		int value = prop.get_value(objInstance).to_int();
		ImGui::InputInt(prop.get_name().c_str(), &value);
		prop.set_value(objInstance, value);
	}
	else if (prop.get_type() == rttr::type::get<float>()) {
		float value = prop.get_value(objInstance).to_float();
		ImGui::InputFloat(prop.get_name().c_str(), &value);
		prop.set_value(objInstance, value);
	}
	else if (prop.get_type() == rttr::type::get<bool>()) {
		bool value = prop.get_value(objInstance).to_bool();
		ImGui::Checkbox(prop.get_name().c_str(), &value);
		prop.set_value(objInstance, value);
	}
	else if (prop.get_type() == rttr::type::get<glm::vec2>()) {
		glm::vec2 value = prop.get_value(objInstance).get_value<glm::vec2>();
		ImGui::DragFloat2(prop.get_name().c_str(), &value[0], 0.1f);
		prop.set_value(objInstance, value);
	}
	else if (prop.get_type() == rttr::type::get<glm::vec3>()) {
		glm::vec3 value = prop.get_value(objInstance).get_value<glm::vec3>();
		ImGui::DragFloat3(prop.get_name().c_str(), &value[0], 0.1f);
		prop.set_value(objInstance, value);
	}
	else if (prop.get_type() == rttr::type::get<glm::vec4>()) {
		glm::vec4 value = prop.get_value(objInstance).get_value<glm::vec4>();
		ImGui::DragFloat4(prop.get_name().c_str(), &value[0], 0.1f);
		prop.set_value(objInstance, value);
	}
	else if (prop.get_type() == rttr::type::get<glm::mat4>()) {
		glm::mat4 value = prop.get_value(objInstance).get_value<glm::mat4>();
		ImGui::Text(prop.get_name().c_str());
		for (int i = 0; i < 4; i++) {
			ImGui::DragFloat4(("Row " + std::to_string(i + 1)).c_str(), &value[i][0]);
		}
		prop.set_value(objInstance, value);
	}
	else if (prop.get_type() == rttr::type::get<std::string>()) {
		std::string value = prop.get_value(objInstance).get_value<std::string>();
		ImGui::InputText(prop.get_name().c_str(), &value);
		prop.set_value(objInstance, value);
	}
	else {
		ImGui::Text("Variable of type %s is not supported!", prop.get_type().get_name().c_str());
	}
}

void PropertyDrawer::DrawFunction(rttr::method& method, rttr::variant& objInstance)
{
	if (ImGui::Button(method.get_name().c_str(), ImVec2(-1, 0))) {
		method.invoke(objInstance);
	}
}
