#pragma once

#include <Components/Component.h>
#include <Macros.h>
#include <icons/IconsFontAwesome6.h>
#include <Components/Rigidbody.h>
#include "CameraComponent.h"

CLASS()
class FirstPersonController : public Component
{
	REFLECT(Component)

	PROPERTY()
	float speed;

	PROPERTY()
	float runningSpeed;

	PROPERTY()
	float sensitivity;

	float pitch;
	
	float cameraFOV;

	Rigidbody* rigidbody;
	CameraComponent* camera;

	glm::vec3 rotation;

	public:
		virtual void Init() override;
		virtual void Update(float dt) override;

		virtual std::string GetIcon() override {
			// 
			return ICON_FA_PERSON;
		}
};

