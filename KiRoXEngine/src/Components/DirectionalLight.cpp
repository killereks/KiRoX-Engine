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

void DirectionalLight::Render(std::vector<MeshComponent*>& meshes)
{
	shadowMap->Render(meshes, GetLightSpaceMatrix());
}

void DirectionalLight::BindShadowMap(int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, shadowMap->GetDepthMap());
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

DirectionalLight::DirectionalLight()
{
	shadowMap = new ShadowMap(4096, 4096);
}

DirectionalLight::~DirectionalLight()
{
	delete shadowMap;
}
