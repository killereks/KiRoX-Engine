#pragma once

#include <glm/glm.hpp>
#include "Component.h"
#include <glm/gtc/matrix_transform.hpp>

#include "../Entity.h"

enum class CameraType
{
	Perspective,
	Orthographic
};

class CameraComponent : public Component
{
	// PERSPECTIVE
	float nearClipPlane;
	float farClipPlane;
	float fieldOfView;

	// ORTHOGRAPHIC
	float left;
	float right;
	float top;
	float bottom;
	float orthoNear;
	float orthoFar;

	CameraType cameraType;

	// TODO: Add a render texture

public:
	CameraComponent();
	~CameraComponent();

	void DrawInspector() override;

	void SetNearClipPlane(float _nearClipPlane) { nearClipPlane = _nearClipPlane; }
	void SetFarClipPlane(float _farClipPlane) { farClipPlane = _farClipPlane; }
	void SetFieldOfView(float _fieldOfView) { fieldOfView = _fieldOfView; }

	const float GetNearClipPlane() const { return nearClipPlane; }
	const float GetFarClipPlane() const { return farClipPlane; }
	const float GetFieldOfView() const { return fieldOfView; }

	glm::mat4 GetViewMatrix() {
		return owner->GetTransform().GetViewMatrix();
	}
	glm::mat4 GetProjectionMatrix() {
		if (cameraType == CameraType::Perspective) {
			return glm::perspective(glm::radians(fieldOfView), 1920.0f / 1080.0f, nearClipPlane, farClipPlane);
		}
		else if (cameraType == CameraType::Orthographic) {
			return glm::ortho(left, right, bottom, top, orthoNear, orthoFar);
		}

		static_assert(true, "Unsupported camera type");
	}
};