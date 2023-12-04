#include "Entity.h"

#include "uuid/uuid_v4.h"

Entity::Entity(std::string name)
{
	this->name = name;

	transformComponent.SetOwner(this);

	UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
	uuid = uuidGenerator.getUUID();
}

Entity::~Entity()
{
	for (auto component : components){
		delete component;
	}

	for (auto child : children){
		delete child;
	}
}

void Entity::SetParent(Entity* _parent)
{
	if (_parent == nullptr)
	{
		return;
	}

	if (parent != nullptr) {
		parent->RemoveChild(this);
	}
	
	_parent->AddChild(this);

	parent = _parent;
}

void Entity::AddChild(Entity* child)
{
	children.push_back(child);
}

void Entity::RemoveChild(Entity* child)
{
	children.erase(std::remove(children.begin(), children.end(), child), children.end());
}