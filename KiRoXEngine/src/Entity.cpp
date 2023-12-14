#include "Entity.h"

#include "uuid/uuid_v4.h"
#include "refl.gen.h"

Entity::Entity(const std::string& name)
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

void Entity::CopyFrom(Entity* otherEntity)
{
	// first copy the transform
	TransformComponent& otherTransform = otherEntity->GetTransform();

	// delete all components
	for (auto component : components) {
		delete component;
	}

	components.clear();

	glm::vec3 pos = otherTransform.GetLocalPosition();
	glm::quat rot = otherTransform.GetLocalRotation();
	glm::vec3 scale = otherTransform.GetLocalScale();
	GetTransform().SetLocalPosition(pos);
	GetTransform().SetLocalRotation(rot);
	GetTransform().SetLocalScale(scale);

	// then copy every component individually
	SetName(otherEntity->GetName());

	for (Component* comp : otherEntity->GetAllComponents()) {
		std::string compName = comp->GetName();

		const rttr::type compType = Reflection::GetType(compName);

		Component* newComponentInstance = Reflection::CreateComponent(compName);
		rttr::variant thisVar = newComponentInstance;
		thisVar.convert(compType);

		Component* otherComponentInstance = comp;
		rttr::variant otherVar = otherComponentInstance;
		otherVar.convert(compType);

		for (rttr::property prop : rttr::type::get_by_name(newComponentInstance->GetName()).get_properties()) {
			std::string name = prop.get_name();
			if (name == "") continue;

			rttr::variant value = prop.get_value(otherVar);
			bool setValueSuccess = prop.set_value(thisVar, value);
			assert(setValueSuccess);
		}

		AddComponent(newComponentInstance);
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

	assert(_parent != nullptr);
	
	_parent->AddChild(this);

	parent = _parent;
}

void Entity::AddChild(Entity* child)
{
	assert(child != nullptr);
	children.push_back(child);
}

void Entity::RemoveChild(Entity* child)
{
	assert(child != nullptr);
	children.erase(std::remove(children.begin(), children.end(), child), children.end());
}
