#include "Component.h"

#include <Reflection/PropertyDrawer.h>

Component::Component()
{
	owner = nullptr;
}

Component::~Component()
{
}

void Component::SetOwner(Entity* _owner)
{
	owner = _owner;
}

TransformComponent& Component::GetTransform()
{
	return owner->GetTransform();
}

bool Component::DrawInspector()
{
	return false;
}
