#include "TransformComponent.h"

#include "../Entity.h"
#include "imgui.h"

TransformComponent::TransformComponent()
{
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	rotation = glm::quat(glm::vec3(0.0));
	scale = glm::vec3(1.0f, 1.0f, 1.0f);
}

TransformComponent::~TransformComponent()
{

}

void TransformComponent::DrawInspector()
{
	ImGui::Text("Position");
	ImGui::Separator();
	ImGui::DragFloat3("##Position", &position[0], 0.1f);
	ImGui::SameLine();
	if (ImGui::Button("R##Position")) {
		position = glm::vec3(0.0);
	}

	glm::vec3 euler = glm::eulerAngles(rotation) * 180.0f / glm::pi<float>();

	ImGui::Text("Rotation");
	ImGui::Separator();
	ImGui::DragFloat3("##Rotation", &euler[0], 0.1f);
	ImGui::SameLine();
	if (ImGui::Button("R##Euler")) {
		euler = glm::vec3(0.0);
	}

	rotation = glm::quat(euler * glm::pi<float>() / 180.0f);

	ImGui::Text("Scale");
	ImGui::Separator();
	ImGui::DragFloat3("##Scale", &scale[0], 0.1f);

	ImGui::SameLine();
	if (ImGui::Button("R##Scale")) {
		scale = glm::vec3(1.0);
	}
}

void TransformComponent::SetLocalPosition(glm::vec3 pos)
{
	position = pos;
}

void TransformComponent::SetLocalRotation(glm::vec3 euler)
{
	rotation = glm::quat(glm::vec3(euler * glm::pi<float>() / 180.0f));
}

void TransformComponent::SetLocalScale(glm::vec3 scale)
{
	this->scale = scale;
}

void TransformComponent::LookAt(glm::vec3 position)
{
	glm::vec3 direction = glm::normalize(position - GetWorldPosition());
	rotation = glm::quatLookAt(direction, glm::vec3(0.0f, 1.0f, 0.0f));
}

void TransformComponent::Translate(glm::vec3 deltaPos)
{
	position += deltaPos;

	for (Entity* ent : owner->GetChildren())
	{
		ent->GetTransform().Translate(deltaPos);
	}
}

glm::vec3 TransformComponent::GetWorldPosition()
{
	glm::vec3 positionOut = GetLocalPosition();

	Entity* parent = owner->GetParent();

	while (parent != nullptr)
	{
		positionOut += parent->GetTransform().GetLocalPosition();
		parent = parent->GetParent();
	}

	return positionOut;
}

glm::quat TransformComponent::GetWorldRotation()
{
	glm::quat rotationOut = GetLocalRotation();

	Entity* parent = owner->GetParent();

	while (parent != nullptr)
	{
		rotationOut *= parent->GetTransform().GetLocalRotation();
		parent = parent->GetParent();
	}

	return rotationOut;
}

glm::vec3 TransformComponent::GetWorldScale()
{
	glm::vec3 scaleOut = GetLocalScale();

	Entity* parent = owner->GetParent();

	while (parent != nullptr)
	{
		scaleOut *= parent->GetTransform().GetLocalScale();
		parent = parent->GetParent();
	}

	return scaleOut;
}

glm::vec3 TransformComponent::GetLocalPosition()
{
	return position;
}

glm::quat TransformComponent::GetLocalRotation()
{
	return rotation;
}

glm::vec3 TransformComponent::GetLocalScale()
{
	return scale;
}

glm::vec3 TransformComponent::GetForward()
{
	return rotation * glm::vec3(0.0f, 0.0f, 1.0f);
}

glm::vec3 TransformComponent::GetUp()
{
	return rotation * glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::vec3 TransformComponent::GetRight()
{
	return rotation * glm::vec3(1.0f, 0.0f, 0.0f);
}

void TransformComponent::Rotate(glm::vec3 axis, float angleDegrees)
{

}

void TransformComponent::RotateLocal(glm::vec3 axis, float angleDegrees)
{

}
