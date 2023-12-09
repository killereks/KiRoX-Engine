#pragma once

#include "Component.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "gtx/quaternion.hpp"
#include "../Macros.h"
#include "icons/IconsFontAwesome6.h"

class Entity;

CLASS()
class TransformComponent : public Component
{
	REFLECT()

	PROPERTY()
	glm::vec3 position;
	PROPERTY()
	glm::quat rotation;
	PROPERTY()
	glm::vec3 scale;

public:
	TransformComponent();
	~TransformComponent();

	void DrawInspector() override;
	void Serialize(YAML::Emitter& out) override;

	std::string GetIcon() override
	{
		return " " ICON_FA_UP_DOWN_LEFT_RIGHT;
	}

	// TODO: Cache
	glm::mat4 GetModelMatrix() {
		glm::mat4 modelMatrix = glm::mat4(1.0f);

		//modelMatrix = glm::scale(modelMatrix, scale) * glm::toMat4(rotation) * glm::translate(modelMatrix, position);

		// SCALE -> ROTATE -> TRANSLATE
		// TRS

		glm::mat4 matrixT = glm::translate(modelMatrix, GetWorldPosition());
		glm::mat4 matrixR = glm::toMat4(GetWorldRotation());
		glm::mat4 matrixS = glm::scale(modelMatrix, GetWorldScale());

		return matrixT * matrixR * matrixS;
	}

	glm::mat4 GetViewMatrix() const {
		glm::mat4 viewMatrix = glm::mat4(1.0f);

		glm::mat4 matrixT = glm::translate(viewMatrix, position);
		glm::mat4 matrixR = glm::toMat4(rotation);

		return glm::inverse(matrixT * matrixR);
	}

	glm::vec3 LocalToWorldDirection(glm::vec3 localDir)
	{
		return rotation * localDir;
	}

	glm::vec3 WorldToLocalDirection(glm::vec3 worldDir)
	{
		return glm::inverse(rotation) * worldDir;
	}

	void SetLocalPosition(glm::vec3 pos);
	void SetLocalRotation(glm::vec3 euler);
	void SetLocalRotation(glm::quat quat);
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

