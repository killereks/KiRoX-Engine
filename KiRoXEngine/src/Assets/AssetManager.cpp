#include "AssetManager.h"
#include "../Engine.h"

#include "../Math/Mathf.h"
#include "../Math/ImGuiExtensions.h"
#include <Tools/SavingLoading.h>

#include <Reflection/PropertyDrawer.h>

AssetManager* AssetManager::instance = nullptr;

void AssetManager::AddCallback_OnLoaded(std::string assetName, std::function<void()> callback)
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

AssetManager::AssetManager(std::filesystem::path path)
{
    instance = this;

    projectPath = path.string();

    LoadAllAssets();
    LoadAllMetaFiles();

    folderWatcher.watchFolder(path.c_str());
    //folderWatcher.OnFileChanged = OnFileChanged;
    folderWatcher.OnFileChanged = [this](std::wstring_view filename, FolderWatcher::Action action)
        {
            OnFileChanged(filename, action);
        };

    textureTypeLookup = {
        {"Scene", "scene.png"},
        {"MeshFilter", "mesh.png"},
        {"Shader", "layers.png"},
        {"ComputeShader","computeshader.png"},
        {"Material", "sphere.png"}
    };
}

void AssetManager::LoadAllMetaFiles()
{
    PROFILE_FUNCTION()
    // go over every meta file in the project
    for (const auto& entry : std::filesystem::recursive_directory_iterator(projectPath)) {
        std::string extension = entry.path().extension().string();

        if (extension == ".meta") {
            // see if it has a corresponding asset file
            std::string assetFilePath = entry.path().string().substr(0, entry.path().string().size() - 5);

            if (!std::filesystem::exists(assetFilePath) || std::filesystem::is_directory(assetFilePath)) {
                std::filesystem::remove(entry.path());
                std::cout << "Removed meta file: " << entry.path().string() << std::endl;
                ImGui::InsertNotification({ ImGuiToastType::Warning, 3000, "Removed meta file: %s", entry.path().filename().string().c_str()});
            }
        }
    }

    for (auto asset : assets) {
        LoadOrCreateMetaFile(*asset.second);
    }
}

void AssetManager::LoadOrCreateMetaFile(Asset& asset)
{
    PROFILE_FUNCTION()
    std::string path = asset.filePath + ".meta";

    if (!std::filesystem::exists(path)) {
        asset.SaveMetaFile();
        ImGui::InsertNotification({ ImGuiToastType::Info, 3000, "Created meta file: %s", asset.name.c_str()});
    }

    asset.LoadMetaFile();

    uuidToAssetName[asset.uuid] = asset.fileName;
}

AssetManager::~AssetManager()
{
    for (auto& asset : assets) {
        delete asset.second;
    }
}

void AssetManager::ForceClickAsset(Asset* asset)
{
    selectedAsset = asset;

    // TODO: Make the Assets window focus

    if (selectedAsset != nullptr) {
        std::string relativePath = selectedAsset->filePath.substr(projectPath.size() + 1);

        this->relativePath.clear();
        cachedPaths.clear();

        std::vector<std::string> folders;
        std::string folder = "";
        for (char c : relativePath) {
            if (c == '.') break;

            if (c == '\\') {
                this->relativePath.push_back(folder);
				folder = "";
			}
            else {
				folder += c;
			}
        }
    }
}

void AssetManager::OnFileChanged(std::wstring_view filename, FolderWatcher::Action action)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::wstring wstr = std::wstring(filename);
    std::string filenameStr = converter.to_bytes(wstr);

    std::cout << "Detected change in file: " << filenameStr << "\n";

    if (action == FolderWatcher::Action::Modified) {
        // reload this asset, delete and create new one
        std::string fileAssetName = filenameStr.substr(filenameStr.find_last_of("\\") + 1);

        if (filenameStr.find(".") != std::string::npos) {
            std::string extension = filenameStr.substr(filenameStr.find_last_of("."));

            std::cout << "File " << fileAssetName << " has been modified\n";

            if (extension == ".shader") {
                Get<Shader>(fileAssetName)->Recompile();
            }
            else if (extension == ".computeshader") {
                Get<ComputeShader>(fileAssetName)->Recompile();
            }
        }
    }
    else if (action == FolderWatcher::Action::Created) {
        LoadAsset(filenameStr);
    }

    // TODO: Handle renaming
}

void AssetManager::Update()
{
    PROFILE_FUNCTION()
//#if EDITOR
    folderWatcher.update();
//#endif
    AsyncUpdate();
}

void AssetManager::DrawInspector()
{
    ImGui::Begin("Assets");

    if (loadedAssets < totalAssets) {
        ImGui::Text("Loading %i / %i assets...", loadedAssets, totalAssets);
        ImGui::SameLine();
        ImGui::ProgressBar((float)loadedAssets / (float)totalAssets, ImVec2(0.0f, 0.0f));
    }

    ////////////////////
    // Drawing Bread crumbs
    std::string currentPath = projectPath + "\\";
    for (std::string folder : relativePath) {
        currentPath += folder + "\\";
    }

    if (cachedPaths.size() == 0) {
        std::filesystem::path path(currentPath);
        for (auto& entry : std::filesystem::directory_iterator(path)) {
            cachedPaths.push_back(entry);
        }

        std::sort(cachedPaths.begin(), cachedPaths.end(), [](const std::filesystem::path& a, const std::filesystem::path& b) {
            if (std::filesystem::is_directory(a) != std::filesystem::is_directory(b)) {
                return std::filesystem::is_directory(a);
            }
            return a < b;
            });
    }

    if (ImGui::Button(ICON_FA_FOLDER" Project")) {
        relativePath.clear();
        cachedPaths.clear();
    }
    ImGui::SameLine();
    ImGui::Text("/");
    ImGui::SameLine();

    for (int i = 0; i < relativePath.size(); ++i) {
        if (ImGui::Button((ICON_FA_FOLDER" " + relativePath[i]).c_str())) {
            relativePath.erase(relativePath.begin() + i + 1, relativePath.end());
            cachedPaths.clear();
            break;
        }
        ImGui::SameLine();
        ImGui::Text("/");
        ImGui::SameLine();
    }
    ImGui::Separator();

    int index = 0;

    ////////////////////
    // Drawing Grid

    int step = 5;
    // round to nearest 5.0f
    fileViewSize = (int)(fileViewSize / step) * step;
    ImGui::SliderFloat("##FileViewSize", &fileViewSize, 5.0f, 250.0f);

    const float padding = 10.0f;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(padding, padding));

    ImVec2 size = ImVec2(fileViewSize, fileViewSize);

    // TODO: Find a better way to do this
    // For some reason the scaling is always off by 50.0f;
    size.x = glm::max(0.0f, size.x - 50.0f);
    size.y = glm::max(0.0f, size.y - 50.0f);

    bool small = false;

    if (fileViewSize < 10.0f) {
        size = ImVec2(-1, 0);
        small = true;
    }

    float maxX = ImGui::GetWindowContentRegionMax().x;
    float currentX = ImGui::GetCursorPosX() + padding;

    for (auto& entry : cachedPaths){
        if (entry.is_directory() && entry.path().filename() == "Editor") {
            continue;
        }

        std::string extension = entry.path().extension().string();
        if (extension == ".meta") continue;

        ImGui::PushID(index++);

        ImGui::BeginGroup();

        std::string fileName = entry.path().filename().string();

        if (entry.is_directory()) {
            Texture* tex = Get<Texture>("folder.png");
            if (small) {
                if (ImGui::Button(fileName.c_str(), size)) {
                    relativePath.push_back(fileName);
                    cachedPaths.clear();
                }
            }
            else {
                if (ImGui::ImageButton((void*)tex->GetTextureID(), size)) {
                    relativePath.push_back(fileName);
                    cachedPaths.clear();
                }
            }
        }
        else {
            Asset* asset = Get<Asset>(fileName);

            ImVec2 pos = ImGui::GetCursorScreenPos();

            if (asset && !asset->IsLoaded())
            {
                if (asset->HasLoadingProgress()) {
                    ImGui::ProgressBar(asset->GetLoadingProgress(), ImVec2(fileViewSize, 0.0f));
                }
                else {
                    ImGui::Spinner("##spinner", fileViewSize * 0.5f, 6, ImGui::GetColorU32(ImGuiCol_ButtonHovered));
                }
            }
            else
            {
                unsigned int texID = 0;
                bool flipUVs = false;

                if (Texture* texture = dynamic_cast<Texture*>(asset)) {
                    if (texture->IsLoaded()) {
                        texID = texture->GetTextureID();
                        flipUVs = true;
                    }
                }
                else if (Material* mat = dynamic_cast<Material*>(asset)) {
                    texID = mat->GetPreviewTextureID();
                    flipUVs = true;
                }
                else {
                    if (asset != nullptr) {
                        texture = GetEditorIcon(asset->GetTypeName());
                        texID = texture->GetTextureID();
                    }
                }

                if (texID != 0)
                {
                    if (small) {
                        if (flipUVs) {
							ImGui::Image((ImTextureID)texID, size, ImVec2(0, 1), ImVec2(1, 0));
						}
                        else {
							ImGui::Image((ImTextureID)texID, size);
						}
                        ImGui::SameLine();
                        ImGui::Text(fileName.c_str(), size);
                    }
                    else {
                        if (flipUVs) {
                            ImGui::Image((ImTextureID)texID, size, ImVec2(0, 1), ImVec2(1, 0));
						}
                        else {
							ImGui::Image((ImTextureID)texID, size);
                        }
                    }

                    bool isDragging = ImGui::IsMouseDragging(ImGuiMouseButton_Left);

                    if (!isDragging && ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                        selectedAsset = asset;
                    }
                }
                else
                {
                    ImGui::Button("Missing icon!", size);
                }

                if (asset != nullptr && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                    Asset** assetPointer = &asset;
                    ImGui::SetDragDropPayload("ASSET", assetPointer, sizeof(assetPointer));
                    ImGui::Text("Dragging %s", asset->fileName.c_str());
                    ImGui::EndDragDropSource();
                }
            }

            if (asset == selectedAsset) {
                float padding = 3.0f;
                pos.x -= padding;
                pos.y -= padding;
                ImGui::GetWindowDrawList()->AddRect(pos, 
                        ImVec2( pos.x + size.x + padding * 2.0f, 
                                pos.y + size.y + padding * 2.0f), 
                        IM_COL32(255, 255, 0, 255));
            }
        }

        Mathf::RenderTextWithEllipsis(fileName, size.x);

        //ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + fileViewSize);
        //ImGui::TextWrapped(fileName.c_str());
        //ImGui::PopTextWrapPos();

        ImGui::EndGroup();
        ImGui::PopID();

        if (!small) {
            currentX += fileViewSize + padding;
            if (currentX + padding > maxX) {
                ImGui::NewLine();
                currentX = ImGui::GetCursorPosX() + padding;
            }
            else {
                ImGui::SameLine();
            }
        }
    }

    ImGui::PopStyleVar();

    ImGui::End();
}

void AssetManager::DrawImportSettings()
{
    PROFILE_FUNCTION()
    ImGui::Begin(ICON_FA_GEARS" Import Settings");

    if (selectedAsset == nullptr) {
        ImGui::Text("No asset selected!");
    }
    else {
        ImGui::Text("Import Settings for %s", selectedAsset->fileName.c_str());

        rttr::variant var = selectedAsset;
        const rttr::type type = Reflection::GetType(selectedAsset->GetTypeName());
        var.convert(type);

        if (PropertyDrawer::DrawObject(var)) {
            selectedAsset->SaveMetaFile();
        }
    }

    ImGui::End();
}

void AssetManager::UnloadAsset(const std::string& name)
{
    if (assets.find(name) != assets.end())
    {
        delete assets[name];
        assets.erase(name);
        loadedAssets--;
    }
}

Texture* AssetManager::GetEditorIcon(const std::string& type)
{
    if (textureTypeLookup.find(type) == textureTypeLookup.end()) return nullptr;

    std::string name = textureTypeLookup[type];
    return Get<Texture>(name);
}

void AssetManager::InvokeLoadedCallbacks(std::string& assetName)
{
    std::vector<std::function<void()>> callbacks = onLoadedCallbacks[assetName];

    for (std::function<void()>& callback : callbacks)
    {
        callback();
    }
}

void AssetManager::AsyncUpdate()
{
    PROFILE_FUNCTION()
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

            loadedAssets++;

            asset->loaded = true;
            InvokeLoadedCallbacks(asset->fileName);
        }
    }
}

void AssetManager::LoadAllAssets()
{
    PROFILE_FUNCTION()

    Console::Write("Loading all assets at: " + projectPath);

    for (const auto& entry : std::filesystem::recursive_directory_iterator(projectPath)) {
        const auto& path = entry.path();
        std::string fullPath = path.string();

        LoadAsset(fullPath);
    }
}

void AssetManager::LoadAsset(std::string& fullPath){
    PROFILE_FUNCTION()

    std::string extension = fullPath.substr(fullPath.find_last_of(".") + 1);
    std::string fileName = fullPath.substr(fullPath.find_last_of("\\") + 1);

    if (extension == "png" || extension == "jpeg" || extension == "jpg")
    {
        Load<Texture>(fileName, fullPath);
    }
    else if (extension == "shader")
    {
        Load<Shader>(fileName, fullPath);
    }
    else if (extension == "fbx" || extension == "obj" || extension == "glb" || extension == "gltf")
    {
        Load<MeshFilter>(fileName, fullPath);
    }
    else if (extension == "scene") {
        Load<Scene>(fileName, fullPath);
    }
    else if (extension == "computeshader") {
        Load<ComputeShader>(fileName, fullPath);
    }
    else if (extension == "mat") {
        Load<Material>(fileName, fullPath);
    }
}
