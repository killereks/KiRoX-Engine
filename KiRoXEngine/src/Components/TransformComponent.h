#pragma once

#include "Component.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "gtx/quaternion.hpp"

class Entity;

class TransformComponent : public Component
{
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

public:
	TransformComponent();
	~TransformComponent();

	void DrawInspector() override;

	// TODO: Cache
	glm::mat4 GetModelMatrix() const {
		glm::mat4 modelMatrix = glm::mat4(1.0f);

		//modelMatrix = glm::scale(modelMatrix, scale) * glm::toMat4(rotation) * glm::translate(modelMatrix, position);

		// SCALE -> ROTATE -> TRANSLATE
		// TRS

		glm::mat4 matrixT = glm::translate(modelMatrix, position);
		glm::mat4 matrixR = glm::toMat4(rotation);
		glm::mat4 matrixS = glm::scale(modelMatrix, scale);

		return matrixT * matrixR * matrixS;
	}

	glm::mat4 GetViewMatrix() const {
		glm::mat4 viewMatrix = glm::mat4(1.0f);

		glm::mat4 matrixT = glm::translate(viewMatrix, position);
		glm::mat4 matrixR = glm::toMat4(rotation);

		return glm::inverse(matrixT * matrixR);
	}

	void SetLocalPosition(glm::vec3 pos);
	void SetLocalRotation(glm::vec3 euler);
	void SetLocalScale(glm::vec3 scale);

	void LookAt(glm::vec3 position);

	void Translate(glm::vec3 deltaPos);

	glm::vec3 GetWorldPosition();
	glm::quat GetWorldRotation();
	glm::vec3 GetWorldScale();

	glm::vec3 GetLocalPosition();
	glm::quat GetLocalRotation();
	glm::vec3 GetLocalScale();

	glm::vec3 GetForward();
	glm::vec3 GetUp();
	glm::vec3 GetRight();

	void Rotate(glm::vec3 axis, float angleDegrees);
	void RotateLocal(glm::vec3 axis, float angleDegrees);
};

