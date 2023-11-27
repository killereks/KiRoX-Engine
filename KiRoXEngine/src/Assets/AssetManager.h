#pragma once

#include <map>
#include <string>
#include <filesystem>
#include <functional>

#include <future>

#include "../Tools/FolderWatcher.h"

#include "Asset.h"
#include "Texture.h"
#include "Shader.h"
#include "Editor/Console.h"

#include <imgui.h>

class AssetManager {
private:
    std::map<std::string, Asset*> assets;

    std::string projectPath;

    FolderWatcher folderWatcher;

public:
    AssetManager(std::filesystem::path path) {
        projectPath = path.string();

        LoadAllAssets();

		folderWatcher.watchFolder(path.c_str());
        //folderWatcher.OnFileChanged = OnFileChanged;
        folderWatcher.OnFileChanged = [&](std::wstring_view filename, FolderWatcher::Action action)
        {
            OnFileChanged(filename, action);
        };
    }

    ~AssetManager() {
        for (auto& asset : assets) {
			delete asset.second;
		}
	}

	void OnFileChanged(std::wstring_view filename, FolderWatcher::Action action)
	{

	}

    void Update()
    {
        folderWatcher.update();
    }

    void DrawInspector() {
        ImGui::Begin("Assets");

        // draw as a square
        const float size = 150.0f;
        const float padding = 10.0f;
        const float windowWidth = ImGui::GetWindowWidth();
        const int columnCount = (int)(windowWidth / (size + padding));
        const float itemWidth = (windowWidth / columnCount) - padding;

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(padding, padding));

        int index = 0;
        for (auto& asset : assets) {
            ImGui::PushID(index++);

            ImGui::BeginGroup();
            Texture* texture = dynamic_cast<Texture*>(asset.second);
            if (texture && texture->IsLoaded()) {
                ImGui::Image((void*)texture->GetTextureID(), ImVec2(size, size));
            }
            else {
                ImGui::Button("Missing Icon", ImVec2(size, size));
            }
            ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + size);
            ImGui::TextWrapped(asset.first.c_str());
            ImGui::PopTextWrapPos();
            ImGui::EndGroup();

            ImGui::PopID();

            if (index % columnCount != 0) {
                ImGui::SameLine();
            }
            else {
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
            }
        }

        ImGui::PopStyleVar();

        ImGui::End();
    }


public:
    template <typename T>
    void Load(const std::string& name, const std::string& filePath) {
        Console::Write("Loaded asset: "+name, ImVec4(0.278, 0.722, 1.0, 1.0));

        if (assets.find(name) != assets.end()) {
            delete assets[name];
        }

        assets[name] = new T(filePath);
    }

    template <typename T>
    T* Get(const std::string& name) {
        return dynamic_cast<T*>(assets[name]);
    }

    void LoadAllAssets() {
        Console::Write("Loading all assets at: "+projectPath);

        const std::filesystem::path assetsPath = projectPath;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(assetsPath)) {
            const auto& path = entry.path();
            const auto& extension = path.extension();

            if (extension == ".png" || extension == ".jpeg" || extension == ".jpg") {
                Load<Texture>(path.filename().string(), path.string());
            }
            else if (extension == ".shader")
            {
                Load<Shader>(path.filename().string(), path.string());
            }
        }
    }
};


