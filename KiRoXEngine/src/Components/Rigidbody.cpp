#include "Rigidbody.h"

#include "imgui.h"

#include "TransformComponent.h"
#include "Entity.h"

#include <Physics/Physics.h>

void Rigidbody::SetBody(reactphysics3d::RigidBody* body)
{
	this->body = body;

	body->setAngularLockAxisFactor(reactphysics3d::Vector3(
									1 - freezeRotationX, 
									1 - freezeRotationY, 
									1 - freezeRotationZ));
}

void Rigidbody::SetPosition(glm::vec3 position)
{
	if (body == nullptr || physics == nullptr) return;

	reactphysics3d::Transform transform = body->getTransform();
	transform.setPosition(reactphysics3d::Vector3(position.x, position.y, position.z));
	body->setTransform(transform);
}

void Rigidbody::SetRotation(glm::quat rotation)
{
	if (body == nullptr || physics == nullptr) return;

	reactphysics3d::Transform transform = body->getTransform();
	transform.setOrientation(reactphysics3d::Quaternion(rotation.x, rotation.y, rotation.z, rotation.w));
	body->setTransform(transform);
}

void Rigidbody::SetPhysics(Physics* physics)
{
	this->physics = physics;
}

void Rigidbody::SetFriction(float friction)
{
	int colliders = body->getNbColliders();
	for (int i = 0; i < colliders; i++) {
		reactphysics3d::Collider* collider = body->getCollider(i);
		collider->getMaterial().setFrictionCoefficient(friction);
	}
}

void Rigidbody::SetBounciness(float bounciness)
{
	int colliders = body->getNbColliders();
	for (int i = 0; i < colliders; i++) {
		reactphysics3d::Collider* collider = body->getCollider(i);
		collider->getMaterial().setBounciness(bounciness);
	}
}

void Rigidbody::AddForce(glm::vec3 force)
{
	body->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(force.x, force.y, force.z));
}

void Rigidbody::SetVelocity(glm::vec3 velocity)
{
	if (body == nullptr || physics == nullptr) return;

	body->setLinearVelocity(reactphysics3d::Vector3(velocity.x, velocity.y, velocity.z));
}

void Rigidbody::Update(float dt)
{
	TransformComponent& transform = owner->GetTransform();

	reactphysics3d::Transform physicsTransform = body->getTransform();

	//float factor = physics->GetAccumulatedTime() / physics->GetFixedTimeStep();

	//reactphysics3d::Transform physicsTransform = reactphysics3d::Transform::interpolateTransforms(previousTransform, currTransform, factor);

	glm::vec3 newPosition = glm::vec3(physicsTransform.getPosition().x, physicsTransform.getPosition().y, physicsTransform.getPosition().z);
	glm::quat newRotation = glm::quat(physicsTransform.getOrientation().w, 
									  physicsTransform.getOrientation().x, 
									  physicsTransform.getOrientation().y, 
									  physicsTransform.getOrientation().z);

	transform.SetLocalPosition(newPosition);
	transform.SetLocalRotation(newRotation);
}
