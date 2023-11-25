#pragma once

#include "Assets/AssetManager.h"
#include "Editor/Console.h"

class Engine
{
public:
	Engine();
	~Engine();

	std::shared_ptr<AssetManager> assetManager;

	void Start();

	void Render();

};

