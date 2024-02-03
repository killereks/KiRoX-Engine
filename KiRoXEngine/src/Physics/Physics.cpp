#include "Physics.h"

#include "Entity.h"
#include "Components/TransformComponent.h"

#include <Macros.h>
#include <Tools/Stopwatch.h>

#include <iostream>
#include <Editor/Gizmos.h>

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

	//physicsThread = std::thread(&Physics::RunPhysicsThread, this);
}

Physics::~Physics()
{
	running = false;

	//physicsThread.join();
	//physicsCommon.destroyPhysicsWorld(world);
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

	reactphysics3d::Collider* collider = rb->GetBody()->addCollider(boxShape, physicsTransform);
	collider->getMaterial().setBounciness(boxCollider->GetBounciness());
	collider->getMaterial().setFrictionCoefficient(boxCollider->GetFriction());

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

	reactphysics3d::Collider* collider = rb->GetBody()->addCollider(capsuleShape, physicsTransform);
	collider->getMaterial().setBounciness(capsuleCollider->GetBounciness());
	collider->getMaterial().setFrictionCoefficient(capsuleCollider->GetFriction());

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

bool Physics::Raycast(glm::vec3 origin, glm::vec3 direction, float distance, RaycastEvent& hitInfo)
{
	reactphysics3d::Vector3 startPoint = reactphysics3d::Vector3(origin.x, origin.y, origin.z);
	reactphysics3d::Vector3 endPoint = reactphysics3d::Vector3(
											origin.x + direction.x * distance,
											origin.y + direction.y * distance,
											origin.z + direction.z * distance
											);

	reactphysics3d::Ray ray(startPoint, endPoint);

	world->raycast(ray, &hitInfo);

	return hitInfo.HasHit();
}

void Physics::Update(float inDeltaTime)
{
	PROFILE_FUNCTION()
	accumulatedTime += inDeltaTime;
	while (accumulatedTime >= fixedTimeStep) {
		world->update(fixedTimeStep);
		accumulatedTime -= fixedTimeStep;
	}

	reactphysics3d::Vector3 startPoint = reactphysics3d::Vector3(0, 10, 0);
	reactphysics3d::Vector3 endPoint = reactphysics3d::Vector3(20, 0, 0);
	reactphysics3d::Ray ray(startPoint, endPoint);

	RaycastEvent raycastEvent;
	world->raycast(ray, &raycastEvent);

	Gizmos::DrawLine(glm::vec3(startPoint.x, startPoint.y, startPoint.z), raycastEvent.GetWorldPoint(), glm::vec4(1, 0, 0, 1));
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
