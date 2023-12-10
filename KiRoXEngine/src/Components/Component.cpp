#include "Component.h"

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
