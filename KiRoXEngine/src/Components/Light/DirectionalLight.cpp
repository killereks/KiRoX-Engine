#include "DirectionalLight.h"

#include <Editor/Gizmos.h>

#include <icons/IconsFontAwesome6.h>
#include <Tools/StringTools.h>
#include <glm/glm.hpp>
#include <GL/glew.h>

bool DirectionalLight::DrawInspector()
{
	ImGui::Image((ImTextureID)shadowMap->GetDepthMap(), ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));

	return true;
}

void DirectionalLight::OnDrawGizmos()
{
	TransformComponent& transform = owner->GetTransform();

	glm::vec3 worldPos = transform.GetWorldPosition();
	glm::vec3 forward = transform.GetForward();

	Gizmos::DrawRay(worldPos, forward, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
}

std::string DirectionalLight::GetIcon()
{
	return ICON_FA_SUN;
}

glm::mat4 DirectionalLight::GetLightSpaceMatrix()
{
	float nearPlane = 0.1f;
	float farPlane = 100.0f;

	TransformComponent& transform = owner->GetTransform();

	glm::vec3 worldPos = transform.GetWorldPosition();

	glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, nearPlane, farPlane);
	glm::mat4 lightView = glm::lookAt(worldPos, worldPos + transform.GetForward(), transform.GetUp());

	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	return lightSpaceMatrix;
}

glm::mat4 DirectionalLight::GetTightLightSpaceMatrix(CameraComponent* camera)
{
	// Step 1. Calculate the 8 corners of the view frustum in world space
	std::vector<glm::vec3> frustumCorners = camera->GetFrustumCorners();

	// Step 2. Transform the frustum corners to light space
	TransformComponent& transform = GetTransform();
	glm::mat4 lightView = glm::lookAt(transform.GetWorldPosition(), transform.GetWorldPosition() + transform.GetForward(), transform.GetUp());

	for (glm::vec3& corner : frustumCorners) {
		corner = glm::vec3(lightView * glm::vec4(corner, 1.0f));
	}

	float minX = frustumCorners[0].x, maxX = frustumCorners[0].x;
	float minY = frustumCorners[0].y, maxY = frustumCorners[0].y;
	float minZ = frustumCorners[0].z, maxZ = frustumCorners[0].z;

	for (int i = 1; i < frustumCorners.size(); i++) {
		glm::vec3 corner = frustumCorners[i];

		minX = std::min(minX, corner.x);
		maxX = std::max(maxX, corner.x);

		minY = std::min(minY, corner.y);
		maxY = std::max(maxY, corner.y);

		minZ = std::min(minZ, corner.z);
		maxZ = std::max(maxZ, corner.z);
	}

	maxZ += 10.0f;
	minZ -= 10.0f;

	glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, -maxZ, -minZ);

	glm::vec3 min = glm::vec3(minX, minY, minZ);
	glm::vec3 max = glm::vec3(maxX, maxY, maxZ);

	Gizmos::DrawWireCube((max + min) / 2.0f, max - min, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

	return lightProjection * lightView;
}

void DirectionalLight::RenderTightCamera(std::vector<MeshComponent*>& meshes, CameraComponent* camera)
{
	shadowMap->Render(meshes, GetTightLightSpaceMatrix(camera));
}

DirectionalLight::DirectionalLight()
{
	shadowMap = new ShadowMap(4096, 4096);
}

DirectionalLight::~DirectionalLight()
{
	delete shadowMap;
}
