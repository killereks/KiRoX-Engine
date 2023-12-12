#pragma once

#include <map>
#include <string>

#include <locale>
#include <codecvt>

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
#include "ComputeShader.h"

class Engine;

class AssetManager
{
private:
    std::unordered_map<std::string, Asset*> assets;
    std::unordered_map<std::string, std::string> textureTypeLookup;

    std::unordered_map<std::string, std::string> uuidToAssetName;

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

    size_t NumberOfAssetsLoading() { return loadingCoroutines.size(); }

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
            {"Shader", "layers.png"},
            {"ComputeShader","computeshader.png"}
        };
    }

    void LoadUUID();
    void SaveUUID();

    const std::string GetUUID(const std::string& filePath);

    ~AssetManager() {
        for (auto& asset : assets) {
			delete asset.second;
		}

        SaveUUID();
	}

	void OnFileChanged(std::wstring_view filename, FolderWatcher::Action action)
	{
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::wstring wstr = std::wstring(filename);
        std::string filenameStr = converter.to_bytes(wstr);

        if (action == FolderWatcher::Action::Modified) {
            // reload this asset, delete and create new one
            std::string fileAssetName = filenameStr.substr(filenameStr.find_last_of("\\") + 1);
            std::string extension = fileAssetName.substr(fileAssetName.find_last_of("."));

            std::cout << "Reloading " << fileAssetName << "\n";

            if (extension == ".shader") {
                Get<Shader>(fileAssetName)->Recompile();
            }

        }
	}

    void Update()
    {
        folderWatcher.update();
        AsyncUpdate();
    }

    void DrawInspector();

    void UnloadAsset(const std::string& name)
    {
        if (assets.find(name) != assets.end())
        {
			delete assets[name];
			assets.erase(name);
		}
	}

	template <typename T>
	void Load(const std::string& name, const std::string& filePath)
	{
        //std::cout << "Loading " << name << "\n";

		if (assets.find(name) != assets.end())
		{
			delete assets[name];
		}

        T* asset = new T();

        asset->filePath = filePath;
        asset->fileName = name;
        asset->uuid = GetUUID(name);

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

    template<typename T>
    T* GetByUUID(const std::string& uuid) {
        if (uuidToAssetName.find(uuid) != uuidToAssetName.end()) {
			return Get<T>(uuidToAssetName[uuid]);
		}

        std::cout << "ERROR!\n";
        return nullptr;
    }

    void AsyncUpdate()
    {
        for (size_t i = 0; i < loadingCoroutines.size(); ++i)
        {
            loadingCoroutines[i].resume();
			if (loadingCoroutines[i].status() == co::Dead)
			{
                Asset* asset = (Asset*)loadingCoroutines[i].getUserData();
                //std::cout << asset->filePath << " has loaded\n";

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

        LoadUUID();

        Console::Write("Loading all assets at: "+projectPath);

        const std::filesystem::path assetsPath = projectPath;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(assetsPath)) {
            const auto& path = entry.path();
            const auto& extension = path.extension();

            std::string fileName = path.filename().string();
            std::string fullPath = path.string();

			if (extension == ".png" || extension == ".jpeg" || extension == ".jpg")
			{
				Load<Texture>(fileName, fullPath);
			}
			else if (extension == ".shader")
			{
			    Load<Shader>(fileName, fullPath);
			}
            else if (extension == ".fbx" || extension == ".obj")
            {
                Load<MeshFilter>(fileName, fullPath);
            }
            else if (extension == ".scene") {
                Load<Scene>(fileName, fullPath);
            }
            else if (extension == ".computeshader") {
                Load<ComputeShader>(fileName, fullPath);
            }
        }
    }
};