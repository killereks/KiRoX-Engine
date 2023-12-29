#include "Rigidbody.h"

#include "imgui.h"

#include "TransformComponent.h"
#include "Entity.h"

#include <Physics.h>

void Rigidbody::SetBody(reactphysics3d::RigidBody* body)
{
	this->body = body;
}

void Rigidbody::DrawInspector()
{
	ImGui::Checkbox("Is Static?", &isStatic);
}

void Rigidbody::SetPhysics(Physics* physics)
{
	this->physics = physics;
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
