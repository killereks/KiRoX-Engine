#include "CameraFlyComponent.h"
#include <Tools/Input.h>
#include <Entity.h>

void CameraFlyComponent::Update(float dt)
{
	if (!Input::GetMouseButton(0)) return;

	glm::vec2 mouseDelta = Input::GetMouseDelta();

	glm::vec3 translationVector = glm::vec3(0.0f);

	if (Input::GetKey(GLFW_KEY_W))
	{
		translationVector += glm::vec3(0.0f, 0.0f, -1.0f);
	}

	if (Input::GetKey(GLFW_KEY_S))
	{
		translationVector += glm::vec3(0.0f, 0.0f, 1.0f);
	}

	if (Input::GetKey(GLFW_KEY_A))
	{
		translationVector += glm::vec3(-1.0f, 0.0f, 0.0f);
	}

	if (Input::GetKey(GLFW_KEY_D))
	{
		translationVector += glm::vec3(1.0f, 0.0f, 0.0f);
	}

	if (Input::GetKey(GLFW_KEY_SPACE))
	{
		translationVector += glm::vec3(0.0f, 1.0f, 0.0f);
	}

	if (Input::GetKey(GLFW_KEY_LEFT_SHIFT))
	{
		translationVector += glm::vec3(0.0f, -1.0f, 0.0f);
	}

	glm::vec3 transformed = owner->GetTransform().GetForward() * translationVector.z + 
							owner->GetTransform().GetRight() * translationVector.x +
							owner->GetTransform().GetUp() * translationVector.y;

	owner->GetTransform().Translate(transformed * speed * dt);

	rotation.x -= mouseDelta.y * 0.1f;
	rotation.y -= mouseDelta.x * 0.1f;

	owner->GetTransform().SetLocalRotation(rotation);
}
