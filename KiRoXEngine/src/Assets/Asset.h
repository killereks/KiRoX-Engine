#pragma once

#include <string>

#include "coroutines/CoroExtensions.h"

class AssetManager;

class Asset
{
	friend AssetManager;

protected:
	bool loaded = false;

public:
	std::string filePath;
	std::string fileName;

	virtual ~Asset() {}

	virtual co::Coro BeginLoading() = 0;

	bool IsLoaded() const { return loaded; }
};

