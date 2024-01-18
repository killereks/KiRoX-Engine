#pragma once
#include <Components/Component.h>
#include "Collider.h"
#include <Macros.h>
#include "icons/IconsFontAwesome6.h"
#include "Entity.h"

CLASS()
class BoxCollider : public Collider
{
	REFLECT(Collider)

	PROPERTY()
	glm::vec3 offset;
	PROPERTY()
	glm::vec3 scale;

public:
	BoxCollider();

	glm::vec3 GetOffset() {
		return offset;
	}

	glm::quat GetWorldRotation() {
		return owner->GetTransform().GetWorldRotation();
	}

	glm::vec3 GetWorldPosition() {
		return owner->GetTransform().GetWorldPosition() + offset;
	}
	glm::vec3 GetScale() {
		return owner->GetTransform().GetWorldScale() * scale;
	}

	virtual void OnDrawGizmos() override;

	virtual std::string GetIcon() override {
		return ICON_FA_BOX;
	}
};

