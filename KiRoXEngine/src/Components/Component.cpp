#include "Component.h"

Component::Component()
{
	owner = nullptr;
}

Component::~Component()
{

}

void Component::SetOwner(Entity* owner)
{
	this->owner = owner;
}

void Component::DrawInspector()
{

}
