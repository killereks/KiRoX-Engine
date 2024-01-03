#pragma once

#include <rttr/registration>

#include "imgui.h"
#include <string>
#include <Assets/AssetManager.h>

class Asset;
class ObjectPtr;

class PropertyDrawer
{
public:
	static bool DrawProperty(rttr::property& prop, rttr::variant& objInstance);

	static bool DrawSingleProperty(rttr::property& prop, rttr::variant& objInstance);

	static void DrawFunction(rttr::method& method, rttr::variant& objInstance);

	static bool DrawObject(rttr::variant& objInstance);
	
	static bool DrawObjectPtrDragDrop(std::string label, ObjectPtr& objectPtr);

	template<typename T>
	requires std::is_base_of<Asset, T>::value
	static bool DrawAssetDragDrop(T*& asset) {
		bool changed = false;

		if (asset == nullptr) {
			std::string type = typeid(T).name();
			std::string display = "(" + type + ") Drag & Drop Asset Here";

			ImGui::Text(display.c_str());
		}
		else {
			std::string type = asset->GetTypeName();
			std::string assetName = "(" + type + ") " + asset->fileName;

			Texture* tex = AssetManager::GetInstance()->GetEditorIcon(type);

			if (tex != nullptr) {
				ImGui::Image((void*)tex->GetTextureID(), ImVec2(20, 20));
				ImGui::SameLine();
			}
			ImGui::Text(assetName.c_str());
		}

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET")) {
				T* droppedAsset = dynamic_cast<T*>(*(Asset**)payload->Data);
				if (droppedAsset != nullptr) {
					asset = droppedAsset;
					changed = true;
				}
			}
			ImGui::EndDragDropTarget();
		}

		return changed;
	}
};