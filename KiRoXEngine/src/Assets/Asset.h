#pragma once

#include <string>
#include <iostream>

#include "coroutines/CoroExtensions.h"

class AssetManager;

class Asset
{
	friend AssetManager;

	std::string name;

protected:
	bool loaded = false;

public:
	std::string filePath;
	std::string fileName;

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

