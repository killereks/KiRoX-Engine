#pragma once

#include <rttr/registration>

#include "imgui.h"
#include <string>

class Asset;

class PropertyDrawer
{
public:
	static void DrawProperty(rttr::property& prop, rttr::variant& objInstance);

	static void DrawSingleProperty(rttr::property& prop, rttr::variant& objInstance);

	static void DrawFunction(rttr::method& method, rttr::variant& objInstance);

	template<typename T>
	requires std::is_base_of<Asset, T>::value
	static void DrawAssetDragDrop(T*& asset) {
		if (asset == nullptr) {
			std::string type = typeid(T).name();
			std::string display = "(" + type + ") Drag & Drop Asset Here";
			ImGui::Text(display.c_str());
		}
		else {
			std::string type = asset->GetTypeName();
			std::string assetName = "(" + type + ") " + asset->fileName;
			ImGui::Text(assetName.c_str());
		}

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET")) {
				T* droppedAsset = dynamic_cast<T*>(*(Asset**)payload->Data);
				if (droppedAsset != nullptr) {
					asset = droppedAsset;
				}
			}
			ImGui::EndDragDropTarget();
		}
	}
};