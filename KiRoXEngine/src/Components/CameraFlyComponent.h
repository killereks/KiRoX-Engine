#pragma once

#include <Components/Component.h>
#include <Macros.h>
#include <icons/IconsFontAwesome6.h>

CLASS()
class CameraFlyComponent : public Component
{
	REFLECT(Component)

	PROPERTY()
	float speed;

	glm::vec3 rotation;

	public:
		virtual void Update(float dt) override;

		virtual std::string GetIcon() override {
			// 
			return ICON_FA_FEATHER;
		}
};

