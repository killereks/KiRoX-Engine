#include "FirstPersonController.h"
#include <Tools/Input.h>
#include <Entity.h>

#include <Math/Mathf.h>
#include <Editor/Gizmos.h>
#include <physics/Physics.h>

void FirstPersonController::Init()
{
	rigidbody = owner->GetComponent<Rigidbody>();
	camera = owner->GetComponentInChildren<CameraComponent>();

	Input::SetMouseVisibility(false);

	cameraFOV = 60;
}

void FirstPersonController::Update(float dt)
{
	glm::vec2 mouseDelta = Input::GetMouseDelta();
	glm::vec2 input = Input::GetMovementAxis();

	glm::vec3 movementVector = owner->GetTransform().GetForward() * -input.y +
							   owner->GetTransform().GetRight() * input.x;

	pitch -= mouseDelta.y * sensitivity;
	pitch = glm::clamp(pitch, -89.0f, 89.0f);

	if (glm::length(input) > 0.1f) {
		movementVector.y = 0.0f;
		movementVector = glm::normalize(movementVector);

		rigidbody->SetFriction(0.0f);
	}
	else {
		rigidbody->SetFriction(1.0f);
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

	if (Input::GetKeyDown(GLFW_KEY_SPACE)) {
		velocity.y = std::sqrt(std::abs(2.0f * jumpHeight * -9.81f));
	}

	rigidbody->SetVelocity(movementVector + glm::vec3(0.0f, 1.0f, 0.0f) * velocity.y);

	GetOwner()->GetTransform().Rotate(glm::vec3(0.0f, 1.0f, 0.0f), -mouseDelta.x * sensitivity);
	camera->GetOwner()->GetTransform().SetLocalRotation(glm::vec3(pitch, 0.0f, 0.0f));

	cameraFOV = Mathf::Lerp(camera->GetFieldOfView(), cameraFOV, dt * 5.0f);
	camera->SetFieldOfView(cameraFOV);

	Ray ray = camera->ViewportPointToRay(glm::vec2(0.5f, 0.5f));
	RaycastEvent hitInfo;
	if (rigidbody->GetPhysics()->Raycast(ray.origin, ray.direction, 25.0f, hitInfo)) {
		Gizmos::DrawWireSphere(hitInfo.GetWorldPoint(), 0.25f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

		Gizmos::DrawLine(ray.origin, hitInfo.GetWorldPoint(), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		Gizmos::DrawRay(hitInfo.GetWorldPoint(), hitInfo.GetWorldNormal(), glm::vec3(0.0f, 1.0f, 1.0f));
	}
	else {
		Gizmos::DrawRay(ray.origin, ray.direction * 25.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	}
}
