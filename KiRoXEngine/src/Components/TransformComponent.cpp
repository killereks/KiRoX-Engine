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

/*void TransformComponent::DrawInspector()
{
	ImGui::Text("Position");
	ImGui::Separator();
	ImGui::DragFloat3("##Position", &position[0], 0.1f);
	ImGui::SameLine();
	if (ImGui::Button("R##Position")) {
		SetLocalPosition(glm::vec3(0.0));
	}

	glm::vec3 euler = glm::degrees(glm::eulerAngles(rotation));

	ImGui::Text("Rotation");
	ImGui::Separator();
	ImGui::DragFloat3("##Rotation", &euler[0], 0.1f);
	ImGui::SameLine();
	if (ImGui::Button("R##Euler")) {
		rotation = glm::quat(glm::vec3(0.0));
	}

	rotation = glm::quat(glm::radians(euler));

	ImGui::Text("Scale");
	ImGui::Separator();
	ImGui::DragFloat3("##Scale", &scale[0], 0.1f);

	ImGui::SameLine();
	if (ImGui::Button("R##Scale")) {
		SetLocalScale(glm::vec3(1.0));
	}
}*/

void TransformComponent::DrawInspector()
{
	ImGui::Text("Position");
	ImGui::Separator();
	ImGui::DragFloat3("##Position", &position[0], 0.1f);
	ImGui::SameLine();
	if (ImGui::Button("R##Position")) {
		SetLocalPosition(glm::vec3(0.0));
	}

	glm::vec3 euler = glm::degrees(glm::eulerAngles(rotation));

	ImGui::Text("Rotation");
	ImGui::Separator();
	ImGui::DragFloat3("##Rotation", &euler[0], 0.1f);
	ImGui::SameLine();
	if (ImGui::Button("R##Euler")) {
		rotation = glm::quat(glm::vec3(0.0));
	}

	rotation = glm::quat(glm::radians(euler));

	ImGui::Text("Scale");
	ImGui::Separator();
	ImGui::DragFloat3("##Scale", &scale[0], 0.1f);

	ImGui::SameLine();
	if (ImGui::Button("R##Scale")) {
		SetLocalScale(glm::vec3(1.0));
	}
}

void TransformComponent::SetLocalPosition(glm::vec3 pos)
{
	position = pos;
	//if (owner->GetParent() == nullptr)
	//{
	//	position = pos;
	//}
	//else
	//{
	//	position = owner->GetParent()->GetTransform().GetWorldPosition() + pos;
	//}
}

void TransformComponent::SetLocalRotation(glm::vec3 euler)
{
	rotation = glm::quat(glm::vec3(euler) * glm::pi<float>() / 180.0f);
	//glm::quat quatRotation = glm::quat(euler * glm::pi<float>() / 180.0f);
	//if (owner->GetParent() == nullptr)
	//{
	//	rotation = quatRotation;
	//}
	//else
	//{
	//	rotation = owner->GetParent()->GetTransform().GetWorldRotation() * quatRotation;
	//}
}

void TransformComponent::SetLocalRotation(glm::quat quat)
{
	rotation = quat;
	//if (owner->GetParent() == nullptr)
	//{
	//	rotation = quat;
	//}
	//else
	//{
	//	rotation = owner->GetParent()->GetTransform().GetWorldPosition() * quat;
	//}
}

void TransformComponent::SetLocalScale(glm::vec3 scale)
{
	//this->scale = scale;
	if (owner->GetParent() == nullptr)
	{
		this->scale = scale;
	}
	else
	{
		this->scale = owner->GetParent()->GetTransform().GetWorldScale() * scale;
	}
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
	return position;
}

glm::quat TransformComponent::GetWorldRotation()
{
	return rotation;
}

glm::vec3 TransformComponent::GetWorldScale()
{
	return scale;
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
	rotation *= glm::angleAxis(glm::radians(angleDegrees), axis);

	for (Entity* ent : owner->GetChildren())
	{
		ent->GetTransform().Rotate(axis, angleDegrees);
	}
}
