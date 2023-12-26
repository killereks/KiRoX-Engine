#include "TransformComponent.h"

#include "../Entity.h"
#include "imgui.h"

TransformComponent::TransformComponent()
{
	position = glm::vec3(0.0f);
	rotation = glm::quat();
	scale = glm::vec3(1.0f);
}

void TransformComponent::DrawInspector()
{
	ImGui::Text("Position");
	ImGui::Separator();
	ImGui::DragFloat3("##Position", &position[0], 0.1f);
	ImGui::SameLine();
	if (ImGui::Button("R##PositionReset")) {
		SetLocalPosition(glm::vec3(0.0));
	}

	glm::vec3 euler = glm::degrees(glm::eulerAngles(rotation));

	ImGui::Text("Rotation");
	ImGui::Separator();
	if (ImGui::DragFloat3("##Rotation", &rotation[0], 0.1f)) {
		rotation = glm::quat(glm::radians(euler));
	}
	ImGui::SameLine();
	if (ImGui::Button("R##EulerReset")) {
		SetLocalRotation(glm::quat());
	}

	ImGui::Text("Scale");
	ImGui::Separator();
	ImGui::DragFloat3("##Scale", &scale[0], 0.1f);

	ImGui::SameLine();
	if (ImGui::Button("R##ScaleReset")) {
		SetLocalScale(glm::vec3(1.0));
	}
}

void TransformComponent::SetWorldPosition(glm::vec3 pos)
{
	Entity* parent = owner->GetParent();
	if (parent) {
		position = pos - parent->GetTransform().GetLocalPosition();
	}
	else {
		position = pos;
	}
}

void TransformComponent::SetWorldRotation(glm::vec3 euler)
{
	glm::quat newWorldRotation = glm::quat(euler * glm::pi<float>() / 180.0f);
	SetWorldRotation(newWorldRotation);
}

void TransformComponent::SetWorldRotation(glm::quat quat)
{
	Entity* parent = owner->GetParent();
	if (parent) {
		rotation = glm::inverse(parent->GetTransform().GetWorldRotation()) * quat;
	}
	else {
		rotation = quat;
	}
	rotation = glm::normalize(rotation);
}

void TransformComponent::SetWorldScale(glm::vec3 scale)
{
	Entity* parent = owner->GetParent();
	if (parent) {
		this->scale = scale / glm::clamp(parent->GetTransform().GetWorldScale(), 0.001f, FLT_MAX);
	}
	else {
		this->scale = scale;
	}
}

void TransformComponent::SetLocalPosition(glm::vec3 pos)
{
	this->position = pos;
}

void TransformComponent::SetLocalRotation(glm::vec3 euler)
{
	this->rotation = glm::quat(euler * glm::pi<float>() / 180.0f);
}

void TransformComponent::SetLocalRotation(glm::quat quat)
{
	this->rotation = quat;
	rotation = glm::normalize(rotation);
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
}

glm::vec3 TransformComponent::GetWorldPosition() const {
	Entity* parent = owner->GetParent();
	if (parent) {
		return parent->GetTransform().GetWorldPosition() + position;
	}
	return position;
}

glm::quat TransformComponent::GetWorldRotation() const {
	Entity* parent = owner->GetParent();
	if (parent) {
		return parent->GetTransform().GetWorldRotation() * rotation;
	}
	return rotation;
}

glm::vec3 TransformComponent::GetWorldScale() const {
	Entity* parent = owner->GetParent();
	if (parent) {
		return parent->GetTransform().GetWorldScale() * scale;
	}
	return scale;
}

glm::vec3 TransformComponent::GetLocalPosition() const { return position;}

glm::quat TransformComponent::GetLocalRotation() const { return rotation;}

glm::vec3 TransformComponent::GetLocalScale() const { return scale; }

glm::vec3 TransformComponent::GetForward() const {	return rotation * glm::vec3(0.0f, 0.0f, 1.0f);}

glm::vec3 TransformComponent::GetUp() const {	return rotation * glm::vec3(0.0f, 1.0f, 0.0f);}

glm::vec3 TransformComponent::GetRight() const {	return rotation * glm::vec3(1.0f, 0.0f, 0.0f);}

void TransformComponent::Rotate(glm::vec3 axis, float angleDegrees)
{	
	rotation *= glm::angleAxis(glm::radians(angleDegrees), axis);

	for (Entity* ent : owner->GetChildren())
	{
		ent->GetTransform().Rotate(axis, angleDegrees);
	}
}

void TransformComponent::Rotate(glm::quat quat)
{
	rotation *= quat;
}

void TransformComponent::RotateLocal(glm::vec3 axis, float angleDegrees)
{
	glm::quat localRot = glm::angleAxis(glm::radians(angleDegrees), axis);
	rotation *= localRot;
}

void TransformComponent::Scale(float scalar)
{
	scale *= scalar;
}

void TransformComponent::Scale(glm::vec3 amount)
{
	scale *= amount;
}
