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

    Asset* selectedAsset = nullptr;

    int totalAssets = 0;
    int loadedAssets = 0;

public:
	static AssetManager* GetInstance()
	{
		return instance;
	}

    static std::string GenerateUUID() {
        UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
        UUIDv4::UUID uuid = uuidGenerator.getUUID();

        return uuid.str();
    }

    Texture* GetWhiteTexture() {
        return Get<Texture>("WhiteSquare.png");
	}

    void AddCallback_OnLoaded(std::string assetName, std::function<void()> callback);

    size_t NumberOfAssetsLoading() { return loadingCoroutines.size(); }

    AssetManager(std::filesystem::path path);

    void LoadAllMetaFiles();
    void LoadOrCreateMetaFile(Asset& asset);

    ~AssetManager();

    void ForceClickAsset(Asset* asset);

    void OnFileChanged(std::wstring_view filename, FolderWatcher::Action action);

    void Update();

    void DrawInspector();
    void DrawImportSettings();

    void UnloadAsset(const std::string& name);

    Texture* GetEditorIcon(const std::string& type);

    template<typename T>
    std::vector<T*> GetAssetsOfType() {
        std::vector<T*> result;

        for (auto& asset : assets) {
            if (dynamic_cast<T*>(asset.second) != nullptr) {
				result.push_back(dynamic_cast<T*>(asset.second));
			}
		}

		return result;
    }

	template <typename T>
	void Load(const std::string& name, const std::string& filePath)
	{
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
        else {
            loadedAssets++;
        }
        totalAssets++;

		Console::Write("Loading asset: " + name, ImVec4(0.278f, 0.722f, 1.0f, 1.0f));
	}

    void InvokeLoadedCallbacks(std::string& assetName);

public:
    template <typename T>
    T* Get(const std::string& name) {
        if (assets.find(name) == assets.end()) {
			//std::cout << "Error, no asset found with name: " << name << "\n";
			return nullptr;
		}
        return dynamic_cast<T*>(assets[name]);
    }

    template<typename T>
    T* GetByUUID(const std::string& uuid) {
        if (uuidToAssetName.find(uuid) != uuidToAssetName.end()) {
			return Get<T>(uuidToAssetName[uuid]);
		}

        //std::cout << "Error, no asset found with uuid: " << uuid << "\n";
        return nullptr;
    }

    void AsyncUpdate();

    void LoadAllAssets();
    void LoadAsset(std::string& fullpath);
};