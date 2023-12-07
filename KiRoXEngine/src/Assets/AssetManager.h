#pragma once

#include <map>
#include <string>
#include <filesystem>
#include <functional>
#include <typeindex>

#include "../Tools/FolderWatcher.h"
#include "../Macros.h"
#include "../Tools/Stopwatch.h"

#include "Asset.h"
#include "Texture.h"
#include "Shader.h"
#include "MeshFilter.h"
#include "../Scene.h"

#include "../Editor/Console.h"

#include "coroutines/CoroExtensions.h"

#include <imgui.h>

class Engine;

class AssetManager
{
private:
    std::unordered_map<std::string, Asset*> assets;
    std::unordered_map<std::string, std::string> textureTypeLookup;

    std::vector<co::Coro> loadingCoroutines;

    std::string projectPath;
    std::vector<std::string> relativePath;

    FolderWatcher folderWatcher;

    std::unordered_map<std::string, std::vector<std::function<void()>>> onLoadedCallbacks;

    static AssetManager* instance;

    float fileViewSize = 100.0f;
    std::vector<std::filesystem::directory_entry> cachedPaths;

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

        textureTypeLookup = {
            {"Scene", "scene.png"},
            {"MeshFilter", "mesh.png"},
            {"Shader", "layers.png"}
        };
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

    void DrawInspector();

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
            else if (extension == ".scene") {
                Load<Scene>(path.filename().string(), path.string());
            }
        }
    }
};