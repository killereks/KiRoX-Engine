#pragma once

#include <glm/glm.hpp>
#include "Component.h"
#include <glm/gtc/matrix_transform.hpp>

#include "../Entity.h"
#include "MeshComponent.h"
#include "../Assets/Shader.h"
#include <vector>
#include "../Tools/RenderTexture.h"
#include "../Math/Plane.h"
#include "../Math/Frustum.h"

enum class CameraType
{
	Perspective,
	Orthographic
};

CLASS()
class CameraComponent : public Component
{
	REFLECT(Component)

	// PERSPECTIVE
	PROPERTY()
	float nearClipPlane;
	PROPERTY()
	float farClipPlane;
	PROPERTY()
	float fieldOfView;

	// ORTHOGRAPHIC
	PROPERTY()
	float left;
	PROPERTY()
	float right;
	PROPERTY()
	float top;
	PROPERTY()
	float bottom;
	PROPERTY()
	float orthoNear;
	PROPERTY()
	float orthoFar;

	float aspect;
	int width;
	int height;

	CameraType cameraType;

	Frustum frustumCache;

	RenderTexture* renderTexture = nullptr;

public:
	CameraComponent();
	~CameraComponent();

	virtual void OnDrawGizmos() override;

	bool DrawInspector() override;

	void RenderGizmos();

	void PreRender();
	void PostRender();

	void Bind();
	void Unbind();

	void Clear();

	bool IsOnOrForwardPlane(Plane& plane, Bounds& bounds);
	bool IsInFrustum(Bounds& bounds);

	void UpdateFrustumCache();

	std::string GetIcon() override
	{
		return " " ICON_FA_CAMERA;
	}

	void SetNearClipPlane(float _nearClipPlane) { nearClipPlane = _nearClipPlane; }
	void SetFarClipPlane(float _farClipPlane) { farClipPlane = _farClipPlane; }
	void SetFieldOfView(float _fieldOfView) { fieldOfView = _fieldOfView; }

	const float GetNearClipPlane() const { return nearClipPlane; }
	const float GetFarClipPlane() const { return farClipPlane; }
	const float GetFieldOfView() const { return fieldOfView; }

	void CreateRenderTexture(int width, int height);

	unsigned int GetRenderTextureID()
	{
		if (renderTexture == nullptr) return 0;

		return renderTexture->GetTextureID();
	}

	unsigned int GetRenderTextureDepthID()
	{
		if (renderTexture == nullptr) return 0;

		return renderTexture->GetDepthTextureID();
	}

	void Render(std::vector<MeshComponent*>& meshes, Shader* shader);
	void Resize(int width, int height);

	glm::mat4 GetViewMatrix() {
		return owner->GetTransform().GetViewMatrix();
	}
	glm::mat4 GetProjectionMatrix() {
		if (cameraType == CameraType::Perspective) {
			return glm::perspective(glm::radians(fieldOfView), aspect, nearClipPlane, farClipPlane);
		}
		else if (cameraType == CameraType::Orthographic) {
			return glm::ortho(left, right, bottom, top, orthoNear, orthoFar);
		}

		static_assert(true, "Unsupported camera type");
	}
};