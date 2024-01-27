#pragma once
#include <Components/Component.h>
#include <Macros.h>

CLASS()
class Collider : public Component
{
	REFLECT(Component)

	virtual std::string GetIcon() = 0;

protected:
	PROPERTY()
	bool isTrigger;

	PROPERTY()
	float bounciness;

	PROPERTY()
	float friction;

public:
	float GetBounciness() const { return bounciness; }
	float GetFriction() const { return friction; }

};

