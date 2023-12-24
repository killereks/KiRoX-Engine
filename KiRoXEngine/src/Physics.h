#pragma once

#include "Components/Rigidbody.h"
#include "Components/BoxCollider.h"

#include "reactphysics3d/engine/PhysicsCommon.h"
#include "reactphysics3d/engine/PhysicsWorld.h"

class Physics
{
	reactphysics3d::PhysicsCommon physicsCommon;
	reactphysics3d::PhysicsWorld* world;

public:
	Physics();
	~Physics();

	reactphysics3d::Transform GetTransform(Entity* ent);

	void RegisterRigidbody(Rigidbody* rb);
	void RegisterCollisionBody(BoxCollider* boxCollider, Rigidbody* rb);

	void CleanUp();

	void Update(float inDeltaTime);
};

