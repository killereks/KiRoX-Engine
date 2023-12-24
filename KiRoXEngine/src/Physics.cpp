#include "Physics.h"

#include "Entity.h"
#include "Components/TransformComponent.h"

#include <iostream>

Physics::Physics()
{
	reactphysics3d::PhysicsWorld::WorldSettings settings;
	settings.defaultVelocitySolverNbIterations = 20;
	settings.isSleepingEnabled = true;
	settings.gravity = reactphysics3d::Vector3(0, -1, 0);
	settings.defaultBounciness = 0.2f;

	world = physicsCommon.createPhysicsWorld(settings);
}

Physics::~Physics()
{
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

	Entity* owner = rb->GetOwner();

	if (owner->HasComponent<BoxCollider>()) {
		BoxCollider* boxCollider = owner->GetComponent<BoxCollider>();

		RegisterCollisionBody(boxCollider, rb);
	}

	std::cout << "Registered a new body! " << rb->GetOwner()->GetName() << "\n";
}

void Physics::RegisterCollisionBody(BoxCollider* boxCollider, Rigidbody* rb) {
	glm::vec3 scale = boxCollider->GetScale();

	reactphysics3d::BoxShape* boxShape = physicsCommon.createBoxShape(reactphysics3d::Vector3(scale.x, scale.y, scale.z));
	rb->GetBody()->addCollider(boxShape, reactphysics3d::Transform::identity());

	std::cout << "Found a box collider. Linking rigidbody to the collider " << rb->GetOwner()->GetName() << "\n";
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
	for (int i = 0; i < 20; i++) {
		world->update(inDeltaTime);
	}
}

void Physics::CleanUp()
{

}
