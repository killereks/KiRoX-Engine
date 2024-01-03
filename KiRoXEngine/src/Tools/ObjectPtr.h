#pragma once

#include <Macros.h>

#include <string>

#include <Assets/AssetManager.h>

template<typename T>
class ObjectPtr {

	std::string uuid;

	T* ptr = nullptr;

public:
	ObjectPtr() = default;
	ObjectPtr(T* ptr) : ptr(ptr) {}

	T* Get() {
		if (ptr == nullptr) {
			ptr = AssetManager::GetInstance()->GetByUUID<T>(uuid);

			if (ptr == nullptr) {
				std::cout << "Error, no asset found with uuid: " << uuid << "\n";
			}
		}

		return ptr;
	}

	void SetUUID(const std::string& uuid) {
		this->uuid = uuid;
		ptr = nullptr;
	}

	T* operator->() const { return ptr; }
	T& operator*() const { return *ptr; }

	bool operator==(const ObjectPtr<T>& other) const { return ptr == other.ptr; }
	bool operator!=(const ObjectPtr<T>& other) const { return ptr != other.ptr; }
};

