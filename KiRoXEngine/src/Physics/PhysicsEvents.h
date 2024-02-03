#pragma once

#include <reactphysics3d/reactphysics3d.h>
#include <glm/glm.hpp>

class PhysicsEvents : public reactphysics3d::EventListener
{
	virtual void onContact(const reactphysics3d::CollisionCallback::CallbackData& data) override;
	virtual void onTrigger(const reactphysics3d::OverlapCallback::CallbackData& data) override;
};

class RaycastEvent : public reactphysics3d::RaycastCallback {

	glm::vec3 worldPoint;
	glm::vec3 worldNormal;

	float distance;

	bool hit = false;

	virtual reactphysics3d::decimal notifyRaycastHit(const reactphysics3d::RaycastInfo& raycastInfo) override;

public:
	inline glm::vec3 GetWorldPoint() { return worldPoint; }
	inline glm::vec3 GetWorldNormal() { return worldNormal; }

	inline float GetDistance() { return distance; }

	inline bool HasHit() { return hit; }
};