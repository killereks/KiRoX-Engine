#pragma once
#include "Component.h"
#include <Macros.h>

CLASS()
class Collider : public Component
{
	REFLECT(Component)

	virtual std::string GetIcon() = 0;

protected:
	PROPERTY()
	bool isTrigger;

};

