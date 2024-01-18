#pragma once

#include "Components/Component.h"
#include "Components/TransformComponent.h"
#include <vector>
#include <string>

#include <memory>

#include "uuid/uuid_v4.h"

#include <yaml-cpp/yaml.h>

class Entity
{
	std::vector<Component*> components;

	TransformComponent transformComponent;

	std::string name;

	Entity* parent;
	std::vector<Entity*> children;

	UUIDv4::UUID uuid;

public:
	Entity(const std::string& name);
	~Entity();

	const std::string& GetName() const { return name; }
	void SetName(std::string newName) { name = newName; }

	const std::vector<Entity*> GetChildren() const { return children; }
	Entity* GetParent() const { return parent; }

	TransformComponent& GetTransform() { return transformComponent; }

	void CopyFrom(Entity* other);

	const std::vector<Component*>& GetAllComponents() const {
		return components;
	}

	const std::vector<Component*> GetAllComponentsWithTransform() {
		std::vector<Component*> componentsOut;
		componentsOut.push_back(&transformComponent);

		for (Component* comp : components) {
			componentsOut.push_back(comp);
		}

		return componentsOut;
	}

	const UUIDv4::UUID GetUUID () const { return uuid; }
	void SetUUID(const UUIDv4::UUID& uuid)
	{
		this->uuid = uuid;
	}

	bool IsAncestorOf(Entity* potentialAncestor) {
		if (parent == nullptr) {
			return false;
		}
		if (parent == potentialAncestor) return true;

		return parent->IsAncestorOf(potentialAncestor);
	}

	void SetParent(Entity* parent);

	void AddChild(Entity* child);
	void RemoveChild(Entity* child);

	template<typename T>
	T* GetComponentInChildren() {
		static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
		for (auto component : components) {
			T* castedComponent = dynamic_cast<T*>(component);
			if (castedComponent != nullptr)
			{
				return castedComponent;
			}
		}

		for (auto child : children) {
			T* castedComponent = child->GetComponentInChildren<T>();
			if (castedComponent != nullptr)
			{
				return castedComponent;
			}
		}

		return nullptr;
	}

	template<typename T>
	T* AddComponent() {
		static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
		auto component = new T();
		component->SetOwner(this);
		components.push_back(component);
		return component;
	}

	void AddComponent(Component* comp) {
		comp->SetOwner(this);
		components.push_back(comp);
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

	void RemoveComponent(Component* componentToDelete) {
		for (auto component : components) {
			if (component == componentToDelete)
			{
				components.erase(std::remove(components.begin(), components.end(), componentToDelete), components.end());
				delete componentToDelete;
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

