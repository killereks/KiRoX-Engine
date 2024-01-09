#include "PropertyDrawer.h"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <glm/glm.hpp>
#include <iostream>

#include <Assets/Asset.h>
#include <Assets/Shader.h>
#include <Assets/Texture.h>
#include <refl.gen.h>

#include <Tools/ObjectPtr.h>

bool PropertyDrawer::DrawProperty(rttr::property& prop, rttr::variant& objInstance)
{
	// TODO: Add array support
	// TODO: Add enum support

	return DrawSingleProperty(prop, objInstance);
}

/*bool PropertyDrawer::DrawPropertyEnum(rttr::property& prop, rttr::variant& objInstance) {
	rttr::enumeration enumType = prop.get_enumeration();
	std::string enumName = enumType.get_name().c_str();

	int value = prop.get_value(objInstance).to_int();
	if (ImGui::BeginCombo(prop.get_name().c_str(), enumType.value_to_name(value).c_str())) {
		for (int i = 0; i < enumType.get_values().size(); i++) {
			std::string name = enumType.value_to_name(i).c_str();
			bool isSelected = value == i;
			if (ImGui::Selectable(name.c_str(), isSelected)) {
				prop.set_value(objInstance, i);
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
		return true;
	}
	return false;
}*/

bool PropertyDrawer::DrawSingleProperty(rttr::property& prop, rttr::variant& objInstance)
{
	if (prop.get_type() == rttr::type::get<int>()) {
		int value = prop.get_value(objInstance).to_int();
		if (ImGui::InputInt(prop.get_name().c_str(), &value)) {
			prop.set_value(objInstance, value);
			return true;
		}
		return false;
	}
	else if (prop.get_type() == rttr::type::get<float>()) {
		float value = prop.get_value(objInstance).to_float();
		if (ImGui::InputFloat(prop.get_name().c_str(), &value)) {
			prop.set_value(objInstance, value);
			return true;
		}
		return false;
	}
	else if (prop.get_type() == rttr::type::get<bool>()) {
		bool value = prop.get_value(objInstance).to_bool();
		if (ImGui::Checkbox(prop.get_name().c_str(), &value)) {
			prop.set_value(objInstance, value);
			return true;
		}
		return false;
	}
	else if (prop.get_type() == rttr::type::get<glm::vec2>()) {
		glm::vec2 value = prop.get_value(objInstance).get_value<glm::vec2>();
		if (ImGui::DragFloat2(prop.get_name().c_str(), &value[0], 0.1f)) {
			prop.set_value(objInstance, value);
			return true;
		}
		return false;
	}
	else if (prop.get_type() == rttr::type::get<glm::vec3>()) {
		glm::vec3 value = prop.get_value(objInstance).get_value<glm::vec3>();
		if (ImGui::DragFloat3(prop.get_name().c_str(), &value[0], 0.1f)) {
			prop.set_value(objInstance, value);
			return true;
		}
		return false;
	}
	else if (prop.get_type() == rttr::type::get<glm::vec4>()) {
		glm::vec4 value = prop.get_value(objInstance).get_value<glm::vec4>();
		if (ImGui::DragFloat4(prop.get_name().c_str(), &value[0], 0.1f)) {
			prop.set_value(objInstance, value);
			return true;
		}
		return false;
	}
	else if (prop.get_type() == rttr::type::get<glm::mat4>()) {
		glm::mat4 value = prop.get_value(objInstance).get_value<glm::mat4>();
		ImGui::Text(prop.get_name().c_str());
		bool anyChanged = false;
		for (int i = 0; i < 4; i++) {
			anyChanged = anyChanged || ImGui::DragFloat4(("Row " + std::to_string(i + 1)).c_str(), &value[i][0]);
		}
		prop.set_value(objInstance, value);

		return anyChanged;
	}
	else if (prop.get_type() == rttr::type::get<std::string>()) {
		std::string value = prop.get_value(objInstance).get_value<std::string>();
		if (ImGui::InputText(prop.get_name().c_str(), &value)) {
			prop.set_value(objInstance, value);
			return true;
		}

		return false;
	}
	else if (prop.get_type() == rttr::type::get<ObjectPtr*>()) {
		ObjectPtr* objectPtr = prop.get_value(objInstance).get_value<ObjectPtr*>();

		return DrawObjectPtrDragDrop(prop.get_name(), *objectPtr);
	}
	// ASSETS (Asset*)
	/*else if (prop.get_type() == rttr::type::get<Shader*>()) {
		Shader* value = prop.get_value(objInstance).get_value<Shader*>();
		if (DrawAssetDragDrop<Shader>(value)) {
			prop.set_value(objInstance, value);
			return true;
		}
		return false;
	}
	else if (prop.get_type() == rttr::type::get<Texture*>()) {
		Texture* value = prop.get_value(objInstance).get_value<Texture*>();
		if (DrawAssetDragDrop<Texture>(value)) {
			prop.set_value(objInstance, value);
			return true;
		}
		return false;
	}*/

	ImGui::Text("Variable of type %s is not supported!", prop.get_type().get_name().c_str());

	return false;
}

void PropertyDrawer::DrawFunction(rttr::method& method, rttr::variant& objInstance)
{
	if (ImGui::Button(method.get_name().c_str(), ImVec2(-1, 0))) {
		method.invoke(objInstance);
	}
}

bool PropertyDrawer::DrawObject(rttr::variant& objInstance)
{
	const rttr::type type = objInstance.get_type();

	std::string typeName = type.get_name().c_str();

	bool anyChanged = false;
	for (rttr::property prop : type.get_properties()) {
		bool changed = DrawProperty(prop, objInstance);

		std::string name = prop.get_name().c_str();

		anyChanged |= changed;
	}

	return anyChanged;
}

bool PropertyDrawer::DrawObjectPtrDragDrop(std::string label, ObjectPtr& objectPtr)
{
	bool changed = false;

	if (objectPtr.HasValue()) {
		Asset* asset = objectPtr.Get<Asset>();

		std::string type = asset->GetTypeName();
		std::string assetName = label+" (" + type + ") " + asset->fileName;

		Texture* tex = AssetManager::GetInstance()->GetEditorIcon(type);

		if (tex != nullptr) {
			ImGui::Image((void*)tex->GetTextureID(), ImVec2(20, 20));
			ImGui::SameLine();
		}
		// if can be casted to a texture
		else if (tex = dynamic_cast<Texture*>(asset)) {
			ImGui::Image((void*)tex->GetTextureID(), ImVec2(20, 20));
			ImGui::SameLine();
		}
		ImGui::Text(objectPtr.Get<Asset>()->fileName.c_str());
	}
	else {
		std::string display = "("+label+") Drag & Drop Asset Here";
		ImGui::Text(display.c_str());
	}

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET")) {
			Asset* droppedAsset = *(Asset**)payload->Data;
			if (droppedAsset != nullptr) {
				objectPtr.SetUUID(droppedAsset->uuid);
				changed = true;
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (objectPtr.HasValue()) {
		ImGui::SameLine();
		ImGui::PushID(label.c_str());
		if (ImGui::Button(ICON_FA_DELETE_LEFT)) {
			objectPtr.Clear();
		}
		ImGui::PopID();
	}


	return changed;
}
