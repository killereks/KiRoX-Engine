#pragma once

#include <vector>
#include <string>
#include "Entity.h"
#include <stack>
#include "Assets/Asset.h"

class Scene : public Asset {

	std::vector<Entity*> entities;

	Entity* rootEntity = nullptr;

	// EDITOR STUFF
	Entity* selectedEntity = nullptr;

public:
	Scene();
	~Scene();

	void SelectEntityByIndex(unsigned int index);
	Entity* GetSelectedEntity() { return selectedEntity; }

	virtual co::Coro BeginLoading() override;

	void DrawHierarchy();
	void DrawEntity(Entity* entity);

	std::vector<Entity*> GetEntities() { return entities; }

	void DrawInspector();

	Entity* CreateEntity(std::string name);
	Entity* GetEntityByUUID(const UUIDv4::UUID& uuid);
	void DeleteEntity(Entity* ent);

	void CopyFrom(Scene* other);
	void DuplicateEntity(Entity* entity);

	void CubeScene();

	void SerializeEntity(YAML::Emitter& out, Entity* ent);
	void SaveScene(std::string path);
	void LoadScene(std::string path);

	std::vector<Entity*> GetAllEntities()
	{
		std::vector<Entity*> entitiesOut;

		if (rootEntity == nullptr)
		{
			return entitiesOut;
		}

		std::stack<Entity*> entityStack;
		entityStack.push(rootEntity);

		while (!entityStack.empty())
		{
			Entity* currentEntity = entityStack.top();
			entityStack.pop();

			entitiesOut.push_back(currentEntity);

			for (Entity* child : currentEntity->GetChildren())
			{
				entityStack.push(child);
			}
		}

		return entitiesOut;
	}

	template<typename T>
	T* FindComponentOfType() {
		static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

		std::vector<Entity*> allEntities = GetAllEntities();

		for (Entity* ent : allEntities)
		{
			if (ent->HasComponent<T>())
			{
				return ent->GetComponent<T>();
			}
		}
		return nullptr;
	}

	template<typename T>
	std::vector<T*> FindComponentsOfType() {
		static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

		std::vector<Entity*> allEntities = GetAllEntities();

		std::vector<T*> componentsOut;

		for (Entity* ent : allEntities) {
			if (ent->HasComponent<T>()) {
				componentsOut.push_back((T*) ent->GetComponent<T>());
			}
		}

		return componentsOut;
	}
};

