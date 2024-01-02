#pragma once

#include <string>
#include <iostream>

#include <Macros.h>

#include "coroutines/CoroExtensions.h"

class AssetManager;

CLASS()
class Asset {
	REFLECT()

	friend AssetManager;

	std::string name;

	float loadingProgress = -1.0f;

protected:
	bool loaded = false;

public:
	std::string filePath;
	std::string fileName;
	PROPERTY()
	std::string uuid;

	float GetLoadingProgress() { return loadingProgress; }
	bool HasLoadingProgress() { return loadingProgress >= 0.0f; }
	void SetLoadingProgress(float progress) { loadingProgress = progress; }

	Asset() = default;
	Asset(const Asset& other) = default;

	virtual ~Asset() {}

	virtual const char* GetTypeName() {
		if (!name.empty())
		{
			return name.c_str();
		}

		std::string_view name_view = typeid(*this).name();
		name_view.remove_prefix(name_view.find_first_of(' ') + 1);
		name = name_view;

		return name.c_str();
	}

	virtual co::Coro BeginLoading() = 0;

	bool IsLoaded() const { return loaded; }
};

