#pragma once

#include "Components/Rigidbody.h"
#include "Components/BoxCollider.h"

#include "reactphysics3d/engine/PhysicsCommon.h"
#include "reactphysics3d/engine/PhysicsWorld.h"

#include <thread>
#include <atomic>

class Physics
{
	reactphysics3d::PhysicsCommon physicsCommon;
	reactphysics3d::PhysicsWorld* world;

	std::atomic<float> accumulatedTime;

	const float fixedTimeStep = 1.0f / 50.0f;

	std::thread physicsThread;
	std::atomic<bool> running;

public:
	Physics();
	~Physics();

	float GetFixedTimeStep() { return fixedTimeStep; }
	float GetAccumulatedTime() { return accumulatedTime; }

	reactphysics3d::Transform GetTransform(Entity* ent);

	void RegisterRigidbody(Rigidbody* rb);
	void RegisterCollisionBody(BoxCollider* boxCollider, Rigidbody* rb);

	void Update(float inDeltaTime);

	void RunPhysicsThread();
};