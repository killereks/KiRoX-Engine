#pragma once

#include <reactphysics3d/reactphysics3d.h>

class PhysicsEvents : public reactphysics3d::EventListener
{
	virtual void onContact(const reactphysics3d::CollisionCallback::CallbackData& data) override;
	virtual void onTrigger(const reactphysics3d::OverlapCallback::CallbackData& data) override;
};

