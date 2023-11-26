#include "Entity.h"

Entity::Entity(std::string name)
{
	this->name = name;

	transformComponent = AddComponent<TransformComponent>();
}

Entity::~Entity()
{
	for (auto component : components){
		delete component;
	}

	for (auto child : children){
		delete child;
	}

	delete transformComponent;
}

void Entity::SetParent(Entity* parent)
{
	if (this->parent != nullptr) {
		this->parent->RemoveChild(this);
	}
	
	parent->AddChild(this);

	this->parent = parent;
}

void Entity::AddChild(Entity* child)
{
	children.push_back(child);
}

void Entity::RemoveChild(Entity* child)
{
	children.erase(std::remove(children.begin(), children.end(), child), children.end());
}
