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

	PROPERTY()
	bool freezeRotationX;

	PROPERTY()
	bool freezeRotationY;

	PROPERTY()
	bool freezeRotationZ;

	PROPERTY()
	bool freezePositionX;

	PROPERTY()
	bool freezePositionY;

	PROPERTY()
	bool freezePositionZ;

	reactphysics3d::RigidBody* body;

	Physics* physics;

public:

	bool IsStatic() { return isStatic; }

	inline glm::vec3 GetVelocity() {
		if (body == nullptr) return glm::vec3(0, 0, 0);
		reactphysics3d::Vector3 velocity = body->getLinearVelocity();
		return glm::vec3(velocity.x, velocity.y, velocity.z);
	}

	inline Physics* GetPhysics() {
		return physics;
	}

	void SetBody(reactphysics3d::RigidBody* body);
	inline reactphysics3d::RigidBody* GetBody() { return body; }

	void SetPosition(glm::vec3 position);
	void SetRotation(glm::quat rotation);

	void SetPhysics(Physics* physics);

	void SetFriction(float friction);
	void SetBounciness(float bounciness);

	void AddForce(glm::vec3 force);
	void SetVelocity(glm::vec3 velocity);

	virtual std::string GetIcon() override {
		return ICON_FA_ATOM;
	}

	virtual void Update(float dt) override;
};

