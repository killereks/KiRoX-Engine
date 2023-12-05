#pragma once

#include <map>
#include <string>
#include <filesystem>
#include <functional>

#include "../Tools/FolderWatcher.h"
#include "../Macros.h"
#include "../Tools/Stopwatch.h"

#include "Asset.h"
#include "Texture.h"
#include "Shader.h"
#include "MeshFilter.h"

#include "../Editor/Console.h"

#include "coroutines/CoroExtensions.h"

#include <imgui.h>

class AssetManager
{
private:
    std::unordered_map<std::string, Asset*> assets;

    std::vector<co::Coro> loadingCoroutines;

    std::string projectPath;

    FolderWatcher folderWatcher;

    std::unordered_map<std::string, std::vector<std::function<void()>>> onLoadedCallbacks;

    static AssetManager* instance;

public:
	static AssetManager* GetInstance()
	{
		return instance;
	}

    void AddCallback_OnLoaded(std::string assetName, std::function<void()> callback)
    {
        if (assets.find(assetName) != assets.end())
        {
            if (assets[assetName]->IsLoaded())
            {
                callback();
            }
            else
            {
                onLoadedCallbacks[assetName].push_back(callback);
            }
        }
        else
        {
            onLoadedCallbacks[assetName].push_back(callback);
        }
    }

    int NumberOfAssetsLoading() { return loadingCoroutines.size(); }

    AssetManager(std::filesystem::path path) {
        projectPath = path.string();

        LoadAllAssets();

		folderWatcher.watchFolder(path.c_str());
        //folderWatcher.OnFileChanged = OnFileChanged;
        folderWatcher.OnFileChanged = [this](std::wstring_view filename, FolderWatcher::Action action)
        {
            OnFileChanged(filename, action);
        };

        instance = this;
    }

    ~AssetManager() {
        for (auto& asset : assets) {
			delete asset.second;
		}
	}

	void OnFileChanged(std::wstring_view filename, FolderWatcher::Action action)
	{
        //std::cout << filename << " has changed... " << action << "\n";
	}

    void Update()
    {
        folderWatcher.update();
        AsyncUpdate();
    }

    void DrawInspector() {
        ImGui::Begin("Assets");

        ImGui::Text("Loading %i assets...", NumberOfAssetsLoading());

        // draw as a square
        const float size = 125.0f;
        const float padding = 10.0f;
        const float windowWidth = ImGui::GetWindowWidth();
        const int columnCount = (int)(windowWidth / (size + padding));
        const float itemWidth = (windowWidth / columnCount) - padding;

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(padding, padding));

        int index = 0;
        for (auto& asset : assets) {
            ImGui::PushID(index++);

            ImGui::BeginGroup();
			if (asset.second && !asset.second->IsLoaded())
			{
				ImGui::Text("Loading...");
			}
            else
            {
                Texture* texture = dynamic_cast<Texture*>(asset.second);
                if (texture && texture->IsLoaded())
                {
                    ImGui::Image((void*)texture->GetTextureID(), ImVec2(size, size));
                }
                else
                {
                    ImGui::Button("Missing Icon", ImVec2(size, size));
                }
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

	template <typename T>
	void Load(const std::string& name, const std::string& filePath)
	{
        std::cout << "Loading " << name << "\n";

		if (assets.find(name) != assets.end())
		{
			delete assets[name];
		}

        T* asset = new T();

        asset->filePath = filePath;
        asset->fileName = name;

        assets[name] = asset;

        co::Coro coroutine = asset->BeginLoading();
        if (coroutine.isValid())
        {
			loadingCoroutines.emplace_back(std::move(coroutine));
        }

		Console::Write("Loading asset: " + name, ImVec4(0.278f, 0.722f, 1.0f, 1.0f));
	}

    void InvokeLoadedCallbacks(std::string& assetName)
    {
        std::vector<std::function<void()>> callbacks = onLoadedCallbacks[assetName];

        for (std::function<void()>& callback : callbacks)
        {
            callback();
        }
    }

public:
    template <typename T>
    T* Get(const std::string& name) {
        return dynamic_cast<T*>(assets[name]);
    }

    void AsyncUpdate()
    {
        for (size_t i = 0; i < loadingCoroutines.size(); ++i)
        {
            loadingCoroutines[i].resume();
			if (loadingCoroutines[i].status() == co::Dead)
			{
                Asset* asset = (Asset*)loadingCoroutines[i].getUserData();
                std::cout << asset->filePath << " has loaded\n";

                std::swap(loadingCoroutines[i], loadingCoroutines.back());
                loadingCoroutines.pop_back();
				--i;

				asset->loaded = true;
				InvokeLoadedCallbacks(asset->fileName);
			}
        }
    }

    void LoadAllAssets() {
        PROFILE_FUNCTION()

        Console::Write("Loading all assets at: "+projectPath);

        const std::filesystem::path assetsPath = projectPath;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(assetsPath)) {
            const auto& path = entry.path();
            const auto& extension = path.extension();

			if (extension == ".png" || extension == ".jpeg" || extension == ".jpg")
			{
				Load<Texture>(path.filename().string(), path.string());
			}
			else if (extension == ".shader")
			{
			    Load<Shader>(path.filename().string(), path.string());
			}
            else if (extension == ".fbx" || extension == ".obj")
            {
                Load<MeshFilter>(path.filename().string(), path.string());
            }
        }
    }
};