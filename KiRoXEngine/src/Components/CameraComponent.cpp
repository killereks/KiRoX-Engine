#include "CameraComponent.h"

#include "imgui.h"
#include "../Macros.h"
#include "../Tools/Stopwatch.h"
#include "../Editor/Gizmos.h"
#include "../Assets/AssetManager.h"
#include <Assets/Material.h>
#include <Rendering/SkyBox.h>

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

	useFrustumCulling = true;

	renderTexture = new RenderTexture(640, 480);
	aspect = (float)640 / (float)480;

	std::vector<Texture*> skyboxTextures;
	skyboxTextures.push_back(AssetManager::GetInstance()->Get<Texture>("px.png"));
	skyboxTextures.push_back(AssetManager::GetInstance()->Get<Texture>("nx.png"));
	skyboxTextures.push_back(AssetManager::GetInstance()->Get<Texture>("py.png"));
	skyboxTextures.push_back(AssetManager::GetInstance()->Get<Texture>("ny.png"));
	skyboxTextures.push_back(AssetManager::GetInstance()->Get<Texture>("pz.png"));
	skyboxTextures.push_back(AssetManager::GetInstance()->Get<Texture>("nz.png"));

	skybox = new SkyBox(skyboxTextures);
}

CameraComponent::~CameraComponent()
{
	delete renderTexture;
}

bool CameraComponent::DrawInspector() {
	ImGui::Checkbox("Use Frustum Culling", &useFrustumCulling);

	if (cameraType == CameraType::Perspective) {
		ImGui::DragFloat("Near Clip Plane", &nearClipPlane);
		ImGui::DragFloat("Far Clip Plane", &farClipPlane);

		ImGui::SliderFloat("Field Of View", &fieldOfView, 0.0f, 179.0f);

		if (ImGui::Button("Switch to Orthographic", ImVec2(-1, 0))) {
			cameraType = CameraType::Orthographic;
		}
	}
	else if (cameraType == CameraType::Orthographic) {
		ImGui::DragFloat("Left", &left);
		ImGui::DragFloat("Right", &right);
		ImGui::Separator();
		ImGui::DragFloat("Top", &top);
		ImGui::DragFloat("Bottom", &bottom);
		ImGui::Separator();
		ImGui::DragFloat("Near", &orthoNear);
		ImGui::DragFloat("Far", &orthoFar);
		ImGui::Separator();
		if (ImGui::Button("Switch to Perspective", ImVec2(-1, 0))) {
			cameraType = CameraType::Perspective;
		}
	}

	return true;
}

void CameraComponent::OnDrawGizmos()
{
	if (cameraType == CameraType::Perspective) {
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
		glm::vec3 nearTopLeft = nearCenter - transform.GetRight() * (nearWidth * 0.5f) + transform.GetUp() * (nearHeight * 0.5f);
		glm::vec3 nearTopRight = nearCenter + transform.GetRight() * (nearWidth * 0.5f) + transform.GetUp() * (nearHeight * 0.5f);

		glm::vec3 nearBottomLeft = nearCenter - transform.GetRight() * (nearWidth * 0.5f) - transform.GetUp() * (nearHeight * 0.5f);
		glm::vec3 nearBottomRight = nearCenter + transform.GetRight() * (nearWidth * 0.5f) - transform.GetUp() * (nearHeight * 0.5f);

		// FAR
		glm::vec3 farTopLeft = farCenter - transform.GetRight() * (farWidth * 0.5f) + transform.GetUp() * (farHeight * 0.5f);
		glm::vec3 farTopRight = farCenter + transform.GetRight() * (farWidth * 0.5f) + transform.GetUp() * (farHeight * 0.5f);

		glm::vec3 farBottomLeft = farCenter - transform.GetRight() * (farWidth * 0.5f) - transform.GetUp() * (farHeight * 0.5f);
		glm::vec3 farBottomRight = farCenter + transform.GetRight() * (farWidth * 0.5f) - transform.GetUp() * (farHeight * 0.5f);

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
	}
	else if (cameraType == CameraType::Orthographic) {
		// draw the frustum
		TransformComponent& transform = owner->GetTransform();

		// ORTHO
		float orthoWidth = right - left;
		float orthoHeight = top - bottom;

		glm::vec3 orthoCenter = transform.GetWorldPosition() - transform.GetForward() * orthoNear;

		// Calculate frustum corners
		glm::vec3 nearTopLeft = orthoCenter - transform.GetRight() * (orthoWidth * 0.5f) + transform.GetUp() * (orthoHeight * 0.5f);
		glm::vec3 nearTopRight = orthoCenter + transform.GetRight() * (orthoWidth * 0.5f) + transform.GetUp() * (orthoHeight * 0.5f);
		glm::vec3 nearBottomLeft = orthoCenter - transform.GetRight() * (orthoWidth * 0.5f) - transform.GetUp() * (orthoHeight * 0.5f);
		glm::vec3 nearBottomRight = orthoCenter + transform.GetRight() * (orthoWidth * 0.5f) - transform.GetUp() * (orthoHeight * 0.5f);

		glm::vec3 farTopLeft = nearTopLeft - transform.GetForward() * orthoFar;
		glm::vec3 farTopRight = nearTopRight - transform.GetForward() * orthoFar;
		glm::vec3 farBottomLeft = nearBottomLeft - transform.GetForward() * orthoFar;
		glm::vec3 farBottomRight = nearBottomRight - transform.GetForward() * orthoFar;

		// GIZMOS
		// Draw near plane
		Gizmos::DrawLine(nearTopLeft, nearTopRight, glm::vec3(1.0));
		Gizmos::DrawLine(nearTopRight, nearBottomRight, glm::vec3(1.0));
		Gizmos::DrawLine(nearBottomRight, nearBottomLeft, glm::vec3(1.0));
		Gizmos::DrawLine(nearBottomLeft, nearTopLeft, glm::vec3(1.0));

		// Draw far plane
		Gizmos::DrawLine(farTopLeft, farTopRight, glm::vec3(1.0));
		Gizmos::DrawLine(farTopRight, farBottomRight, glm::vec3(1.0));
		Gizmos::DrawLine(farBottomRight, farBottomLeft, glm::vec3(1.0));
		Gizmos::DrawLine(farBottomLeft, farTopLeft, glm::vec3(1.0));

		// Draw connecting lines
		Gizmos::DrawLine(nearTopLeft, farTopLeft, glm::vec3(1.0));
		Gizmos::DrawLine(nearTopRight, farTopRight, glm::vec3(1.0));
		Gizmos::DrawLine(nearBottomLeft, farBottomLeft, glm::vec3(1.0));
		Gizmos::DrawLine(nearBottomRight, farBottomRight, glm::vec3(1.0));
	}
}

void CameraComponent::RenderGizmos()
{
	Gizmos::GetInstance()->Draw(this);
}

void CameraComponent::PreRender()
{
	Bind();
	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	Clear();
	glViewport(0, 0, width, height);

	skybox->Bind(11);
	skybox->Draw(this);
	skybox->Unbind();
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

void CameraComponent::ClearColor(glm::vec4 color)
{
	if (renderTexture != nullptr)
	{
		renderTexture->ClearColor(color);
	}
}

unsigned int CameraComponent::GetSkyboxTextureID()
{
	return skybox->GetTextureID();
}

bool CameraComponent::IsOnOrForwardPlane(Plane& plane, Bounds& bounds)
{
	glm::vec3 extents = bounds.GetSize();

	const float r = extents.x * std::abs(plane.normal.x) + extents.y * std::abs(plane.normal.y) + extents.z * std::abs(plane.normal.z);

	return -r <= plane.GetSignedDistanceToPlane(bounds.GetCenter());
}

bool CameraComponent::IsInFrustum(Bounds& bounds)
{
	UpdateFrustumCache();
	
	return  IsOnOrForwardPlane(frustumCache.leftFace,	bounds)	&&
			IsOnOrForwardPlane(frustumCache.farFace,	bounds)	&&
			IsOnOrForwardPlane(frustumCache.rightFace,	bounds)	&&
			IsOnOrForwardPlane(frustumCache.leftFace,	bounds)	&&
			IsOnOrForwardPlane(frustumCache.topFace,	bounds)	&&
			IsOnOrForwardPlane(frustumCache.bottomFace, bounds);
}

void CameraComponent::UpdateFrustumCache()
{
	TransformComponent& transform = owner->GetTransform();

	const float halfVside = farClipPlane * glm::tan(glm::radians(fieldOfView * 0.5f));
	const float halfHside = halfVside * aspect;
	const glm::vec3 frontMultFar = farClipPlane * transform.GetForward();

	frustumCache.nearFace = { transform.GetWorldPosition() + nearClipPlane * transform.GetForward(), transform.GetForward() };
	frustumCache.farFace = { transform.GetWorldPosition() + frontMultFar, -transform.GetForward() };

	frustumCache.rightFace = { transform.GetWorldPosition(), glm::cross(frontMultFar - transform.GetRight() * halfHside, transform.GetUp()) };
	frustumCache.leftFace = { transform.GetWorldPosition(), glm::cross(transform.GetUp(), frontMultFar + transform.GetRight() * halfHside) };

	frustumCache.topFace = { transform.GetWorldPosition(), glm::cross(transform.GetRight(), frontMultFar - transform.GetUp() * halfVside) };
	frustumCache.bottomFace = { transform.GetWorldPosition(), glm::cross(frontMultFar + transform.GetUp() * halfVside, transform.GetRight()) };
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
	glm::mat4 viewMatrix = GetViewMatrix();
	glm::mat4 projectionMatrix = GetProjectionMatrix();

	shader->setMat4("perspectiveMatrix", projectionMatrix);
	shader->setMat4("viewMatrix", viewMatrix);
	
	skybox->Bind(11);
	shader->setInt("skybox", 11);

	// render
	for (MeshComponent* meshComp : meshes)
	{
		Material* material = meshComp->GetMaterial();

		if (useFrustumCulling) {
			Bounds* bounds = meshComp->GetBounds();
			if (bounds != nullptr && !IsInFrustum(*bounds)) {
				StatsCounter::GetInstance()->IncreaseCounter("culledTriangles", meshComp->GetTriangleCount());
				continue;
			}
		}

		meshComp->SimpleDraw(shader);
	}
}

void CameraComponent::RenderUsingMaterials(std::vector<MeshComponent*>& meshes)
{
	glm::mat4 viewMatrix = GetViewMatrix();
	glm::mat4 projectionMatrix = GetProjectionMatrix();

	for (MeshComponent* meshComp : meshes) {
		Material* material = meshComp->GetMaterial();

		if (material == nullptr) {
			// some default material
			continue;
		}

		if (useFrustumCulling) {
			Bounds* bounds = meshComp->GetBounds();
			if (bounds != nullptr && !IsInFrustum(*bounds)) {
				StatsCounter::GetInstance()->IncreaseCounter("culledTriangles", meshComp->GetTriangleCount());
				continue;
			}
		}

		material->Bind();
		Shader* shader = material->GetShader();

		if (shader == nullptr) {
			// some default shader
			continue;
		}

		shader->setMat4("perspectiveMatrix", projectionMatrix);
		shader->setMat4("viewMatrix", viewMatrix);

		meshComp->SimpleDraw(shader);
	}
}

void CameraComponent::ForceRenderWithMaterial(MeshComponent* mesh, Material* material)
{
	glm::mat4 viewMatrix = GetViewMatrix();
	glm::mat4 projectionMatrix = GetProjectionMatrix();

	if (material == nullptr) {
		std::cout << "Material is null" << std::endl;
		return;
	}

	material->Bind();

	Shader* shader = material->GetShader();

	if (shader == nullptr) {
		std::cout << "Shader is null" << std::endl;
		return;
	}

	shader->setMat4("perspectiveMatrix", projectionMatrix);
	shader->setMat4("viewMatrix", viewMatrix);

	mesh->SimpleDraw(shader);
}

void CameraComponent::Resize(int width, int height)
{
	this->width = width;
	this->height = height;
	if (height != 0) {
		aspect = (float)width / (float)height;
	}
	else {
		aspect = 1.0f;
	}
	renderTexture->Resize(width, height);
}
