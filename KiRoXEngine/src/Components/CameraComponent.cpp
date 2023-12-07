#include "CameraComponent.h"

#include "imgui.h"
#include "../Macros.h"
#include "../Tools/Stopwatch.h"
#include "../Editor/Gizmos.h"

#include "../Math/Frustum.h"

CameraComponent::CameraComponent()
{
	nearClipPlane = 0.3f;
	farClipPlane = 1000.0f;
	fieldOfView = 60.0f;

	cameraType = CameraType::Perspective;

	left = -10.0f;
	right = 10.0f;
	top = 10.0f;
	bottom = -10.0f;
	orthoNear = 1.0f;
	orthoFar = 100.0f;

	renderTexture = new RenderTexture(640, 480);
	aspect = (float)640 / (float)480;
}

CameraComponent::~CameraComponent()
{
	delete renderTexture;
}

void CameraComponent::DrawInspector()
{
	if (cameraType == CameraType::Perspective) {
		ImGui::Text("Perspective Camera");
		ImGui::DragFloat("Near Clip Plane", &nearClipPlane, 0.1f, 100.0f);
		ImGui::DragFloat("Far Clip Plane", &farClipPlane, 0.1f, 100.0f);

		ImGui::SliderFloat("Field Of View", &fieldOfView, 1.0f, 179.0f, "%.0f");

		if (ImGui::Button("Change to Orthographic", ImVec2(-1, 0))) {
			cameraType = CameraType::Orthographic;
		}
	}
	else if (cameraType == CameraType::Orthographic) {
		ImGui::Text("Orthographic Camera");

		ImGui::DragFloat("Left", &left, -100.0f, 100.0f);
		ImGui::DragFloat("Right", &right, -100.0f, 100.0f);
		ImGui::DragFloat("Top", &top, -100.0f, 100.0f);
		ImGui::DragFloat("Bottom", &bottom, -100.0f, 100.0f);
		ImGui::DragFloat("Near", &orthoNear, -100.0f, 100.0f);
		ImGui::DragFloat("Far", &orthoFar, -100.0f, 100.0f);

		if (ImGui::Button("Change to Perspective", ImVec2(-1, 0))) {
			cameraType = CameraType::Perspective;
		}
	}

	// draw the frustum
	float halfFOV = glm::radians(fieldOfView) * 0.5f;
	
	float nearHeight = 2.0f * glm::tan(halfFOV) * nearClipPlane;
	float nearWidth = nearHeight * aspect;

	float farHeight = 2.0f * glm::tan(halfFOV) * farClipPlane;
	float farWidth = farHeight * aspect;

	TransformComponent& transform = owner->GetTransform();

	glm::vec3 nearCenter = transform.GetWorldPosition() - transform.GetForward() * nearClipPlane;
	glm::vec3 farCenter = transform.GetWorldPosition() - transform.GetForward() * farClipPlane;

	// NEAR
	glm::vec3 nearTopLeft =		nearCenter - transform.GetRight() * (nearWidth * 0.5f) + transform.GetUp() * (nearHeight * 0.5f);
	glm::vec3 nearTopRight =	nearCenter + transform.GetRight() * (nearWidth * 0.5f) + transform.GetUp() * (nearHeight * 0.5f);

	glm::vec3 nearBottomLeft =	nearCenter - transform.GetRight() * (nearWidth * 0.5f) - transform.GetUp() * (nearHeight * 0.5f);
	glm::vec3 nearBottomRight = nearCenter + transform.GetRight() * (nearWidth * 0.5f) - transform.GetUp() * (nearHeight * 0.5f);

	// FAR
	glm::vec3 farTopLeft =		farCenter - transform.GetRight() * (farWidth * 0.5f) + transform.GetUp() * (farHeight * 0.5f);
	glm::vec3 farTopRight =		farCenter + transform.GetRight() * (farWidth * 0.5f) + transform.GetUp() * (farHeight * 0.5f);

	glm::vec3 farBottomLeft =	farCenter - transform.GetRight() * (farWidth * 0.5f) - transform.GetUp() * (farHeight * 0.5f);
	glm::vec3 farBottomRight =	farCenter + transform.GetRight() * (farWidth * 0.5f) - transform.GetUp() * (farHeight * 0.5f);

	// GIZMOS
	Gizmos::DrawLine(nearTopLeft, nearTopRight, glm::vec3(1.0));
	Gizmos::DrawLine(nearTopRight, nearBottomRight, glm::vec3(1.0));
	Gizmos::DrawLine(nearBottomRight, nearBottomLeft, glm::vec3(1.0));
	Gizmos::DrawLine(nearBottomLeft, nearTopLeft, glm::vec3(1.0));

	Gizmos::DrawLine(farTopLeft, farTopRight, glm::vec3(1.0));
	Gizmos::DrawLine(farTopRight, farBottomRight, glm::vec3(1.0));
	Gizmos::DrawLine(farBottomRight, farBottomLeft, glm::vec3(1.0));
	Gizmos::DrawLine(farBottomLeft, farTopLeft, glm::vec3(1.0));

	Gizmos::DrawLine(nearTopLeft, farTopLeft, glm::vec3(1.0));
	Gizmos::DrawLine(nearTopRight, farTopRight, glm::vec3(1.0));
	Gizmos::DrawLine(nearBottomRight, farBottomRight, glm::vec3(1.0));
	Gizmos::DrawLine(nearBottomLeft, farBottomLeft, glm::vec3(1.0));

	Gizmos::DrawRay(transform.GetWorldPosition(), transform.GetForward(), glm::vec3(1.0, 1.0, 0.0));
}

void CameraComponent::Serialize(YAML::Emitter& out)
{
	SERIALIZE_VALUE(nearClipPlane);
	SERIALIZE_VALUE(farClipPlane);
	SERIALIZE_VALUE(fieldOfView);

	SERIALIZE_VALUE(top);
	SERIALIZE_VALUE(bottom);
	SERIALIZE_VALUE(left);
	SERIALIZE_VALUE(right);
	SERIALIZE_VALUE(orthoNear);
	SERIALIZE_VALUE(orthoFar);
}

void CameraComponent::DrawGizmos()
{
	Gizmos::GetInstance()->Draw(this);
}

void CameraComponent::PreRender()
{
	Bind();
	Clear();
	glViewport(0, 0, width, height);
}

void CameraComponent::PostRender()
{
	Unbind();
}

void CameraComponent::Bind()
{
	if (renderTexture != nullptr)
	{
		renderTexture->Bind();
	}
}

void CameraComponent::Unbind()
{
	if (renderTexture != nullptr)
	{
		renderTexture->Unbind();
	}
}

void CameraComponent::Clear()
{
	if (renderTexture != nullptr)
	{
		renderTexture->Clear();
	}
}

bool CameraComponent::IsOnOrForwardPlane(Plane& plane, Bounds& bounds)
{
	glm::vec3 extents = bounds.GetSize();

	const float r = extents.x * std::abs(plane.normal.x) + extents.y * std::abs(plane.normal.y) + extents.z * std::abs(plane.normal.z);

	return -r <= plane.GetSignedDistanceToPlane(bounds.GetCenter());
}

bool CameraComponent::IsInFrustum(Bounds& bounds)
{
	Frustum frustum;

	TransformComponent& transform = owner->GetTransform();
	
	const float halfVside = farClipPlane * glm::tan(glm::radians(fieldOfView * 0.5f));
	const float halfHside = halfVside * aspect;
	const glm::vec3 frontMultFar = farClipPlane * transform.GetForward();

	frustum.nearFace = { transform.GetWorldPosition() + nearClipPlane * transform.GetForward(), transform.GetForward() };
	frustum.farFace = { transform.GetWorldPosition() + frontMultFar, -transform.GetForward() };
	
	frustum.rightFace = { transform.GetWorldPosition(), glm::cross(frontMultFar - transform.GetRight() * halfHside, transform.GetUp()) };
	frustum.leftFace = { transform.GetWorldPosition(), glm::cross(transform.GetUp(), frontMultFar + transform.GetRight() * halfHside) };

	frustum.topFace = { transform.GetWorldPosition(), glm::cross(transform.GetRight(), frontMultFar - transform.GetUp() * halfVside) };
	frustum.bottomFace = { transform.GetWorldPosition(), glm::cross(frontMultFar + transform.GetUp() * halfVside, transform.GetRight()) };

	return  IsOnOrForwardPlane(frustum.leftFace, bounds) &&
			IsOnOrForwardPlane(frustum.farFace, bounds) &&
			IsOnOrForwardPlane(frustum.rightFace, bounds) &&
			IsOnOrForwardPlane(frustum.leftFace, bounds) &&
			IsOnOrForwardPlane(frustum.topFace, bounds) &&
			IsOnOrForwardPlane(frustum.bottomFace, bounds);
}

void CameraComponent::CreateRenderTexture(int width, int height)
{
	if (renderTexture != nullptr)
	{
		delete renderTexture;
	}

	renderTexture = new RenderTexture(width, height);
}

void CameraComponent::Render(std::vector<MeshComponent*>& meshes, Shader* shader)
{
	shader->use();

	shader->setMat4("perspectiveMatrix", GetProjectionMatrix());
	shader->setMat4("viewMatrix", GetViewMatrix());

	// render
	for (MeshComponent* meshComp : meshes)
	{
		Bounds bounds = meshComp->GetBounds();
		if (!IsInFrustum(bounds)) {
			StatsCounter::GetInstance()->IncreaseCounter("culledVertices", meshComp->GetVertexCount());
			continue;
		}

		meshComp->SimpleDraw(shader);
	}
}

void CameraComponent::Resize(int width, int height)
{
	this->width = width;
	this->height = height;
	aspect = (float)width / (float)height;
	renderTexture->Resize(width, height);
}
