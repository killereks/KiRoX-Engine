#include "FirstPersonController.h"
#include <Tools/Input.h>
#include <Entity.h>

#include <Math/Mathf.h>

void FirstPersonController::Init()
{
	rigidbody = owner->GetComponent<Rigidbody>();
	camera = owner->GetComponentInChildren<CameraComponent>();

	rigidbody->SetBounciness(0.0f);

	cameraFOV = 60;
}

void FirstPersonController::Update(float dt)
{
	glm::vec2 mouseDelta = Input::GetMouseDelta();
	glm::vec2 input = Input::GetMovementAxis();

	glm::vec3 movementVector = owner->GetTransform().GetForward() * -input.y +
							   owner->GetTransform().GetRight() * input.x;

	pitch -= mouseDelta.y * sensitivity * dt;
	pitch = glm::clamp(pitch, -89.0f, 89.0f);

	if (glm::length(input) > 0.1f) {
		movementVector.y = 0.0f;
		movementVector = glm::normalize(movementVector);
	}

	if (Input::GetKey(GLFW_KEY_LEFT_SHIFT)) {
		movementVector *= runningSpeed;
		cameraFOV = 70.0f;
	}
	else {
		movementVector *= speed;
		cameraFOV = 60.0f;
	}

	glm::vec3 velocity = rigidbody->GetVelocity();

	rigidbody->SetVelocity(movementVector + glm::vec3(0.0f, 1.0f, 0.0f) * velocity.y);

	GetOwner()->GetTransform().Rotate(glm::vec3(0.0f, 1.0f, 0.0f), -mouseDelta.x * sensitivity * dt);
	camera->GetOwner()->GetTransform().SetLocalRotation(glm::vec3(pitch, 0.0f, 0.0f));

	cameraFOV = Mathf::Lerp(camera->GetFieldOfView(), cameraFOV, dt * 5.0f);
	camera->SetFieldOfView(cameraFOV);
}
