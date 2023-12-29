#pragma once
#include "Component.h"
#include "../Macros.h"
#include "../icons/IconsFontAwesome6.h"

#include "reactphysics3d/body/RigidBody.h"

class Physics;

CLASS()
class Rigidbody : public Component
{
	REFLECT(Component)

	PROPERTY()
	bool isStatic;

	reactphysics3d::RigidBody* body;

	Physics* physics;

public:

	bool IsStatic() { return isStatic; }

	void SetBody(reactphysics3d::RigidBody* body);
	inline reactphysics3d::RigidBody* GetBody() { return body; }

	void SetPhysics(Physics* physics);

	virtual void DrawInspector() override;
	virtual std::string GetIcon() override {
		return ICON_FA_FLASK;
	}

	virtual void Update(float dt) override;
};

