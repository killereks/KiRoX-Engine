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

    void AddCallback_OnLoaded(std::string assetName, std::function<void()> callback);

    size_t NumberOfAssetsLoading() { return loadingCoroutines.size(); }

    AssetManager(std::filesystem::path path);

    void LoadAllMetaFiles();
    void LoadOrCreateMetaFile(const std::string& filePath);

    const std::string GetUUID(const std::string& filePath);

    ~AssetManager();

    void OnFileChanged(std::wstring_view filename, FolderWatcher::Action action);

    void Update();

    void DrawInspector();

    void UnloadAsset(const std::string& name);

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

    void InvokeLoadedCallbacks(std::string& assetName);

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

    void AsyncUpdate();

    void LoadAllAssets();
};