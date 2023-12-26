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
	if (_parent == nullptr || _parent == this){
		return;
	}

	if (_parent->IsAncestorOf(this)) {
		std::cout << "Trying to parent " << _parent->GetName() << " to its own child!\n";
		return;
	}

	if (parent != nullptr) {
		parent->RemoveChild(this);
	}

	assert(_parent != nullptr);

	// Store current world transformation
	glm::vec3 currentWorldPosition = transformComponent.GetWorldPosition();
	glm::quat currentWorldRotation = transformComponent.GetWorldRotation();
	glm::vec3 currentWorldScale = transformComponent.GetWorldScale();

	_parent->AddChild(this);

	// Recalculate local transformation relative to new parent
	glm::vec3 newLocalPosition = currentWorldPosition - _parent->transformComponent.GetWorldPosition();
	glm::quat newLocalRotation = glm::inverse(_parent->transformComponent.GetWorldRotation()) * currentWorldRotation;
	newLocalRotation = glm::inverse(newLocalRotation);  // Normalize quaternion
	glm::vec3 newLocalScale = currentWorldScale / _parent->transformComponent.GetWorldScale();

	transformComponent.SetLocalPosition(currentWorldPosition);
	transformComponent.SetLocalRotation(currentWorldRotation);
	transformComponent.SetLocalScale(currentWorldScale);

	parent = _parent;
}

void Entity::AddChild(Entity* child)
{
	assert(child != nullptr);
	children.push_back(child);
	child->parent = this;
}

void Entity::RemoveChild(Entity* child)
{
	assert(child != nullptr);
	children.erase(std::remove(children.begin(), children.end(), child), children.end());
	child->parent = nullptr;
}
