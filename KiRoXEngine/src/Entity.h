#pragma once

#include "Components/Component.h"
#include "Components/TransformComponent.h"
#include <vector>
#include <string>

class Entity
{
	std::vector<Component*> components;

	TransformComponent* transformComponent;

	std::string name;

	Entity* parent;
	std::vector<Entity*> children;

public:
	Entity(std::string name);
	~Entity();

	const std::string GetName() const { return name; }

	const std::vector<Entity*> GetChildren() const { return children; }
	const Entity* GetParent() const { return parent; }

	TransformComponent* GetTransform() const { return transformComponent; }

	std::vector<Component*> GetAllComponents() const {
		return components;
	}

	void SetParent(Entity* parent);

	void AddChild(Entity* child);
	void RemoveChild(Entity* child);

	template<typename T>
	T* AddComponent() {
		static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
		T* component = new T();
		component->SetOwner(this);
		components.push_back(component);
		return component;
	}

	template<typename T>
	T* GetComponent() {
		static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
		for (auto component : components) {
			T* castedComponent = dynamic_cast<T*>(component);
			if (castedComponent != nullptr)
			{
				return castedComponent;
			}
		}
		return nullptr;
	}

	template<typename T>
	void RemoveComponent() {
		static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
		for (auto component : components) {
			T* castedComponent = dynamic_cast<T*>(component);
			if (castedComponent != nullptr)
			{
				components.erase(std::remove(components.begin(), components.end(), castedComponent), components.end());
				delete castedComponent;
				return;
			}
		}
	}

	template<typename T>
	bool TryGetComponent(T*& component) {
		static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
		for (auto component : components) {
			T* castedComponent = dynamic_cast<T*>(component);
			if (castedComponent != nullptr)
			{
				component = castedComponent;
				return true;
			}
		}
		return false;
	}

	template<typename T>
	bool HasComponent() {
		static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
		for (auto component : components) {
			T* castedComponent = dynamic_cast<T*>(component);
			if (castedComponent != nullptr)
			{
				return true;
			}
		}
		return false;
	}
};

