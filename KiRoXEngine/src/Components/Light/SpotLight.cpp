#include "SpotLight.h"
#include <icons/IconsFontAwesome6.h>
#include <Editor/Gizmos.h>

SpotLight::SpotLight()
{
	shadowMap = new ShadowMap(2048, 2048);
	angle = 45.0f;
}

SpotLight::~SpotLight()
{
	delete shadowMap;
}

void SpotLight::OnDrawGizmos()
{
	// draw a cone
	glm::vec3 worldPos = owner->GetTransform().GetWorldPosition();

	glm::vec3 forwardVector = owner->GetTransform().GetForward();
	glm::vec3 rightDirVector = owner->GetTransform().GetRight();
	glm::vec3 upDirVector = owner->GetTransform().GetUp();

	float halfAngle = glm::radians(angle * 0.5f);

	glm::vec3 topVector = glm::rotate(glm::angleAxis(halfAngle, rightDirVector), forwardVector);
	glm::vec3 bottomVector = glm::rotate(glm::angleAxis(-halfAngle, rightDirVector), forwardVector);
	glm::vec3 leftVector = glm::rotate(glm::angleAxis(halfAngle, upDirVector), forwardVector);
	glm::vec3 rightVector = glm::rotate(glm::angleAxis(-halfAngle, upDirVector), forwardVector);

	Gizmos::DrawLine(worldPos, worldPos + topVector, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	Gizmos::DrawLine(worldPos, worldPos + bottomVector, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	Gizmos::DrawLine(worldPos, worldPos + leftVector, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	Gizmos::DrawLine(worldPos, worldPos + rightVector, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
}

bool SpotLight::DrawInspector()
{
	ImGui::DragFloat("Angle", &angle, 0.1f, 0.0f, 179.0f);

	ImGui::Image((void*)shadowMap->GetDepthMap(), ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));

	return true;
}

std::string SpotLight::GetIcon()
{
	return ICON_FA_LIGHTBULB;
}

glm::mat4 SpotLight::GetLightSpaceMatrix()
{
	TransformComponent& transform = owner->GetTransform();

	float nearPlane = 0.1f;
	float farPlane = 100.0f;

	glm::mat4 lightProjection = glm::perspective(glm::radians(angle), 1.0f, nearPlane, farPlane);
	glm::mat4 lightView = glm::lookAt(transform.GetWorldPosition(), transform.GetWorldPosition() + transform.GetForward(), transform.GetUp());

	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	return lightSpaceMatrix;
}
