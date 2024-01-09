#include "ObjectPtr.h"

void ObjectPtr::Clear()
{
	uuid = "";
	ptr = nullptr;
}

bool ObjectPtr::HasValue()
{
	if (ptr == nullptr) {
		FindAsset();
	}
	return ptr != nullptr && !uuid.empty();
}

void ObjectPtr::FindAsset() {
	if (uuid.empty()) {
		return;
	}

	assert(AssetManager::GetInstance() != nullptr, "AssetManager is null");

	Asset* asset = AssetManager::GetInstance()->GetByUUID<Asset>(uuid);

	if (asset != nullptr) {
		ptr = asset;

		std::cout << "[ObjectPtr] Found asset with uuid: " << uuid << "\n";
	}
}

std::string ObjectPtr::GetUUID()
{
	return uuid;
}

void ObjectPtr::SetUUID(const std::string& uuid)
{
	this->uuid = uuid;
	FindAsset();
}