#pragma once

#include "Component.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "gtx/quaternion.hpp"
#include "icons/IconsFontAwesome6.h"

class Entity;
class Rigidbody;

CLASS()
class TransformComponent : public Component
{
	friend class Rigidbody;

	REFLECT(Component)

	PROPERTY()
	glm::vec3 position;
	PROPERTY()
	glm::quat rotation;
	PROPERTY()
	glm::vec3 scale;

public:
	TransformComponent();

	bool DrawInspector() override;

	std::string GetIcon() override
	{
		return " " ICON_FA_UP_DOWN_LEFT_RIGHT;
	}

	// TODO: Cache
	glm::mat4 GetModelMatrix() const {
		glm::mat4 modelMatrix = glm::mat4(1.0f);

		// SCALE -> ROTATE -> TRANSLATE
		// TRS

		glm::mat4 matrixT = glm::translate(modelMatrix, GetWorldPosition());
		glm::mat4 matrixR = glm::toMat4(GetWorldRotation());
		glm::mat4 matrixS = glm::scale(modelMatrix, GetWorldScale());

		return matrixT * matrixR * matrixS;
	}

	glm::mat4 GetLocalModelMatrix() const {
		glm::mat4 modelMatrix = glm::mat4(1.0f);

		glm::mat4 matrixT = glm::translate(modelMatrix, GetLocalPosition());
		glm::mat4 matrixR = glm::toMat4(GetLocalRotation());
		glm::mat4 matrixS = glm::scale(modelMatrix, GetLocalScale());

		return matrixT * matrixR * matrixS;
	}

	glm::mat4 GetViewMatrix() const {
		glm::mat4 viewMatrix = glm::mat4(1.0f);

		glm::mat4 matrixT = glm::translate(viewMatrix, GetWorldPosition());
		glm::mat4 matrixR = glm::toMat4(GetWorldRotation());

		return glm::inverse(matrixT * matrixR);
	}

	// Transforms from local to world space
	glm::vec3 TransformPoint(glm::vec3 point) const {
		return GetModelMatrix() * glm::vec4(point.x, point.y, point.z, 1.0);
	}

	// transforms a direction from local space to world space
	glm::vec3 TransformDirection(glm::vec3 point) const {
		return glm::vec4(point.x, point.y, point.z, 1.0f) * rotation;
	}

	// transforms a point from world space to local space
	glm::vec3 InverseTransformPoint(glm::vec3 point) const {
		glm::mat4 worldToLocal = glm::inverse(GetModelMatrix());
		return worldToLocal * glm::vec4(point.x, point.y, point.z, 1.0f);
	}

	// transforms a direction from world to local space
	glm::vec3 InverseTransformDirection(glm::vec3 point) const {
		return glm::vec4(point.x, point.y, point.z, 1.0f) * glm::inverse(rotation);
	}

	glm::vec3 LocalToWorldDirection(glm::vec3 localDir)
	{
		return rotation * localDir;
	}

	glm::vec3 WorldToLocalDirection(glm::vec3 worldDir)
	{
		return glm::inverse(rotation) * worldDir;
	}

	void SetWorldPosition(glm::vec3 pos);
	void SetWorldRotation(glm::vec3 euler);
	void SetWorldRotation(glm::quat quat);
	void SetWorldScale(glm::vec3 scale);

	void SetLocalPosition(glm::vec3 pos);
	void SetLocalRotation(glm::vec3 euler);
	void SetLocalRotation(glm::quat quat);
	void SetLocalScale(glm::vec3 scale);

	void LookAt(glm::vec3 position);

	void Translate(glm::vec3 deltaPos);

	glm::vec3 GetWorldPosition() const;
	glm::quat GetWorldRotation() const;
	glm::vec3 GetWorldScale() const;

	glm::vec3 GetLocalPosition() const;
	glm::quat GetLocalRotation() const;
	glm::vec3 GetLocalScale() const;

	glm::vec3 GetForward() const;
	glm::vec3 GetUp() const;
	glm::vec3 GetRight() const;

	void Rotate(glm::vec3 axis, float angleDegrees);
	void Rotate(glm::quat quat);
	void RotateLocal(glm::vec3 axis, float angleDegrees);

	void Scale(float scalar);
	void Scale(glm::vec3 amount);
};

