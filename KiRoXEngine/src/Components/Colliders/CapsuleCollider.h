#pragma once
#include "Collider.h"

#include <Macros.h>
#include <icons/IconsFontAwesome6.h>

CLASS()
class CapsuleCollider : public Collider
{
	REFLECT(Collider)

	PROPERTY()
	float radius;

	PROPERTY()
	float height;

public:
	inline float GetRadius() { return radius; }
	inline float GetHeight() { return height; }

	virtual void OnDrawGizmos() override;

	virtual std::string GetIcon() override {
		return ICON_FA_CAPSULES;
	}

};

