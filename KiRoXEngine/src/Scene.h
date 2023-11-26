#pragma once

#include <vector>
#include <string>
#include "Entity.h"

class Scene {

	std::vector<Entity*> entities;

	Entity* rootEntity;

	// EDITOR STUFF
	Entity* selectedEntity = nullptr;
	char componentName[32] = "";

public:
	Scene();
	~Scene();

	void DrawHierarchy();
	void DrawEntity(Entity* entity);

	std::vector<Entity*> GetEntities() { return entities; }

	void DrawInspector();

	Entity* CreateEntity(std::string name);
};

