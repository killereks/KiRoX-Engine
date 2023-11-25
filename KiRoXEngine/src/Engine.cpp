#pragma once
#include "Engine.h"

Engine::Engine()
{
}

Engine::~Engine()
{
}

void Engine::Start()
{
	std::filesystem::path path = std::filesystem::current_path();
	path /= "Project";

	assetManager = std::make_shared<AssetManager>(path.string());
}

void Engine::Render()
{
	assetManager.get()->DrawInspector();
	Console::GetInstance()->Draw();
}
