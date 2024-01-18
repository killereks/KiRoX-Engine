#include "Physics.h"

#include "Entity.h"
#include "Components/TransformComponent.h"

#include <Macros.h>
#include <Tools/Stopwatch.h>

#include <iostream>

Physics::Physics()
{
	reactphysics3d::PhysicsWorld::WorldSettings settings;
	settings.defaultVelocitySolverNbIterations = 5;
	settings.isSleepingEnabled = true;
	settings.gravity = reactphysics3d::Vector3(0, reactphysics3d::decimal(-9.81), 0);

	world = physicsCommon.createPhysicsWorld(settings);

	world->setEventListener(&events);

	accumulatedTime = 0.0f;
	running = true;

	physicsThread = std::thread(&Physics::RunPhysicsThread, this);
}

Physics::~Physics()
{
	running = false;

	physicsThread.join();
	physicsCommon.destroyPhysicsWorld(world);
}

void Physics::RegisterRigidbody(Rigidbody* rb)
{
	reactphysics3d::Transform physicsTransform = GetTransform(rb->GetOwner());

	reactphysics3d::RigidBody* body = world->createRigidBody(physicsTransform);

	if (rb->IsStatic()) {
		body->setType(reactphysics3d::BodyType::STATIC);
	}
	else {
		body->setType(reactphysics3d::BodyType::DYNAMIC);
	}

	rb->SetBody(body);
	rb->SetPhysics(this);

	Entity* owner = rb->GetOwner();

	if (owner->HasComponent<BoxCollider>()) {
		BoxCollider* boxCollider = owner->GetComponent<BoxCollider>();

		RegisterCollisionBody(boxCollider, rb);
	}

	if (owner->HasComponent<CapsuleCollider>()) {
		CapsuleCollider* capsuleCollider = owner->GetComponent<CapsuleCollider>();

		RegisterCollisionBody(capsuleCollider, rb);
	}

	//std::cout << "Registered a new body! " << rb->GetOwner()->GetName() << "\n";
}

void Physics::RegisterCollisionBody(BoxCollider* boxCollider, Rigidbody* rb) {
	glm::vec3 scale = boxCollider->GetScale();

	reactphysics3d::BoxShape* boxShape = physicsCommon.createBoxShape(reactphysics3d::Vector3(scale.x, scale.y, scale.z));
	reactphysics3d::Transform physicsTransform = reactphysics3d::Transform::identity();

	glm::vec3 offset = boxCollider->GetOffset();
	physicsTransform.setPosition(reactphysics3d::Vector3(offset.x, offset.y, offset.z));

	rb->GetBody()->addCollider(boxShape, physicsTransform);

	//std::cout << "Found a box collider. Linking rigidbody to the collider " << rb->GetOwner()->GetName() << "\n";
}

void Physics::RegisterCollisionBody(CapsuleCollider* capsuleCollider, Rigidbody* rb)
{
	float radius = capsuleCollider->GetRadius();
	float height = capsuleCollider->GetHeight();

	reactphysics3d::CapsuleShape* capsuleShape = physicsCommon.createCapsuleShape(radius, height);
	reactphysics3d::Transform physicsTransform = reactphysics3d::Transform::identity();

	//glm::vec3 offset = capsuleCollider->GetOffset();
	//physicsTransform.setPosition(reactphysics3d::Vector3(offset.x, offset.y, offset.z));

	rb->GetBody()->addCollider(capsuleShape, physicsTransform);

	//std::cout << "Found a capsule collider. Linking rigidbody to the collider " << rb->GetOwner()->GetName() << "\n";
}

reactphysics3d::Transform Physics::GetTransform(Entity* ent)
{
	TransformComponent& entityTransform = ent->GetTransform();
	glm::vec3 entityPosition = entityTransform.GetWorldPosition();
	glm::quat entityRotation = entityTransform.GetWorldRotation();

	reactphysics3d::Transform transform;

	transform.setPosition(reactphysics3d::Vector3(entityPosition.x, entityPosition.y, entityPosition.z));
	transform.setOrientation(reactphysics3d::Quaternion(entityRotation.x, entityRotation.y, entityRotation.z, entityRotation.w));

	return transform;
}

void Physics::Update(float inDeltaTime)
{
	accumulatedTime += inDeltaTime;
}

void Physics::RunPhysicsThread()
{
	while (running) {
		while (accumulatedTime >= fixedTimeStep) {
			if (!running) break;

			world->update(fixedTimeStep);
			accumulatedTime -= fixedTimeStep;
		}
	}
}
