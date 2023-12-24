#pragma once
#include "Component.h"
#include "../Macros.h"
#include "icons/IconsFontAwesome6.h"
#include "Entity.h"

CLASS()
class BoxCollider : public Component
{
	REFLECT(Component);

	glm::vec3 offset;
	glm::vec3 scale = glm::vec3(1, 1, 1);

public:
	glm::vec3 GetWorldPosition() {
		return owner->GetTransform().GetWorldPosition() + offset;
	}
	glm::vec3 GetScale() {
		return owner->GetTransform().GetWorldScale() * scale;
	}

	virtual void OnDrawGizmos() override;

	virtual void DrawInspector() override;
	virtual std::string GetIcon() override {
		return ICON_FA_SQUARE;
	}
};

