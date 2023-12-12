#include "AssetManager.h"
#include "../Engine.h"

#include "../Math/Mathf.h"
#include "../Math/ImGuiExtensions.h"

AssetManager* AssetManager::instance = nullptr;

void AssetManager::LoadUUID()
{
    std::ifstream file;

    file.open(projectPath + "\\uuids.txt");

    if (file.is_open())
    {
		std::string line;

        while (std::getline(file, line))
        {
			std::string uuid = line.substr(0, line.find_first_of(" "));
			std::string path = line.substr(line.find_first_of(" ") + 1);

            std::string assetName = path.substr(path.find_last_of("\\") + 1);

			uuidToAssetName[uuid] = assetName;
		}
	}

    file.close();
}

void AssetManager::SaveUUID()
{
    std::ofstream file;

	file.open(projectPath + "\\uuids.txt");

    if (file.is_open())
    {
        for (auto& [uuid, path] : uuidToAssetName)
        {
			file << uuid << " " << path << std::endl;
		}
	}

	file.close();
}

const std::string AssetManager::GetUUID(const std::string& filePath)
{
    for (auto& [uuid, _path] : uuidToAssetName)
    {
        if (_path == filePath) {
            return uuid;
        }
    }

    // create new UUID
    UUIDv4::UUID uuid = UUIDv4::UUIDGenerator<std::mt19937_64>().getUUID();
    std::string uuidString = uuid.str();

    std::string assetName = filePath.substr(filePath.find_last_of("\\") + 1);

    uuidToAssetName[uuidString] = assetName;

    return uuidString;
}

void AssetManager::DrawInspector()
{
    ImGui::Begin("Assets");

    ImGui::Text("Loading %i assets...", NumberOfAssetsLoading());

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

    if (relativePath.size() > 0) {
        for (int i = 0; i < relativePath.size() - 1; ++i) {
            if (ImGui::Button((ICON_FA_FOLDER" " + relativePath[i]).c_str())) {
                relativePath.erase(relativePath.begin() + i + 1, relativePath.end());
                cachedPaths.clear();
                break;
            }
            ImGui::SameLine();
            ImGui::Text("/");
            ImGui::SameLine();
        }
    }
    ImGui::Separator();

    int index = 0;

    ////////////////////
    // Drawing Grid

    ImGui::SliderFloat("##FileViewSize", &fileViewSize, 5.0f, 150.0f);

    const float padding = 10.0f;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(padding, padding));

    ImVec2 size = ImVec2(fileViewSize, fileViewSize);

    bool small = false;

    if (fileViewSize < 10.0f) {
        size = ImVec2(-1, 0);
        small = true;
    }

    float width = ImGui::GetContentRegionAvail().x;
    int elementsPerRow = (int) (width / (size.x + padding)) - 2;
    int elementIndex = 0;

    for (auto& entry : cachedPaths){
        if (entry.is_directory() && entry.path().filename() == "Editor") {
            continue;
        }

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

            if (asset && !asset->IsLoaded())
            {
                ImGui::Spinner("##spinner", fileViewSize * 0.5f, 6, ImGui::GetColorU32(ImGuiCol_ButtonHovered));
            }
            else
            {
                Texture* texture = dynamic_cast<Texture*>(asset);

                if (texture == nullptr) {
                    if (asset != nullptr) {
                        std::string name = textureTypeLookup[asset->GetTypeName()];
                        texture = Get<Texture>(name);
                    }
                }

                if (texture && texture->IsLoaded())
                {
                    if (small) {
                        ImGui::Image((void*)texture->GetTextureID(), ImVec2(16, 16));
                        ImGui::SameLine();
                        ImGui::Text(fileName.c_str(), size);
                    }
                    else {
                        ImGui::Image((void*)texture->GetTextureID(), size);
                    }
                }
                else
                {
                    ImGui::Button("Missing icon!", size);
                }

                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                    Asset** assetPointer = &asset;
                    ImGui::SetDragDropPayload("ASSET", assetPointer, sizeof(assetPointer));
                    ImGui::Text("Dragging %s", asset->fileName.c_str());
                    ImGui::EndDragDropSource();
                }
            }
        }

        Mathf::RenderTextWithEllipsis(fileName, size.x);

        //ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + fileViewSize);
        //ImGui::TextWrapped(fileName.c_str());
        //ImGui::PopTextWrapPos();

        ImGui::EndGroup();
        ImGui::PopID();

        if (!small) {
            if ((elementIndex > 0 && (elementIndex % elementsPerRow) == 0)) {
                ImGui::NewLine();
            }
            else {
                ImGui::SameLine();
            }
        }
        elementIndex++;
    }

    ImGui::PopStyleVar();

    ImGui::End();
}
