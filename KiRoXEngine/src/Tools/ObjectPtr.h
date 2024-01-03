#pragma once

#include <Macros.h>
#include <string>
#include <Assets/AssetManager.h>

CLASS()
class ObjectPtr {
	REFLECT()

	PROPERTY()
	std::string uuid;

	void* ptr;

	void FindAsset();
public:
	ObjectPtr() {
		uuid = "";
		ptr = nullptr;
	}

	void Clear();

	template<typename T>
	T* Get() {
		if (ptr == nullptr) {
			FindAsset();

			if (ptr == nullptr) {
				std::cout << "Error, no asset found with uuid: " << uuid << "\n";
			}
		}

		return (T*) ptr;
	}

	bool HasValue();

	void SetUUID(const std::string& uuid);
	std::string GetUUID();
};

