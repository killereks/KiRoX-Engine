#include "TransformComponent.h"

#include "../Entity.h"
#include "imgui.h"

TransformComponent::TransformComponent()
{
	position = glm::vec3(0.0f);
	rotation = glm::quat();
	scale = glm::vec3(1.0f);
}

void DrawColoredVector(glm::vec3& vec, const glm::vec3& resetValue, const char* label) {
	ImGui::PushID(label);

	// Define colors for X, Y, Z buttons
	ImVec4 colorX = ImVec4(0.91f, 0.3f, 0.24f, 1.0f);	// #e74c3c
	ImVec4 colorY = ImVec4(0.3f, 0.79f, 0.31f, 1.0f);   // #4caf50
	ImVec4 colorZ = ImVec4(0.2f, 0.6f, 0.86f, 1.0f);    // #3498db

	float buttonWidth = 30.0f;

	ImGui::TableNextColumn();
	ImGui::Text(label);

	ImGui::TableNextColumn();
	ImGui::PushStyleColor(ImGuiCol_Button, colorX);
	if (ImGui::Button("X", ImVec2(buttonWidth, 0))) {
		vec = resetValue;
	}
	ImGui::PopStyleColor();
	ImGui::SameLine();
	ImGui::DragFloat("##X", &vec.x, 0.1f);

	ImGui::TableNextColumn();
	ImGui::PushStyleColor(ImGuiCol_Button, colorY);
	if (ImGui::Button("Y", ImVec2(buttonWidth, 0))) {
		vec = resetValue;
	}
	ImGui::PopStyleColor();
	ImGui::SameLine();
	ImGui::DragFloat("##Y", &vec.y, 0.1f);

	ImGui::TableNextColumn();
	ImGui::PushStyleColor(ImGuiCol_Button, colorZ);
	if (ImGui::Button("Z", ImVec2(buttonWidth, 0))) {
		vec = resetValue;
	}
	ImGui::PopStyleColor();
	ImGui::SameLine();
	ImGui::DragFloat("##Z", &vec.z, 0.1f);

	ImGui::PopID();
}

bool TransformComponent::DrawInspector()
{
	ImGuiTableFlags flags =
		ImGuiTableFlags_NoPadInnerX |
		ImGuiTableFlags_NoPadOuterX;

	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

	if (ImGui::BeginTable("TransformTable", 4, flags)) {

		glm::vec3 euler = glm::degrees(glm::eulerAngles(rotation));
		// Column 2: Position Input (X, Y, Z)
		DrawColoredVector(position, glm::vec3(0.0f), "Position");
		DrawColoredVector(euler, glm::vec3(0.0f), "Rotation");
		DrawColoredVector(scale, glm::vec3(1.0f), "Scale");

		rotation = glm::quat(glm::radians(euler));

		ImGui::EndTable();
	}

	ImGui::PopStyleVar(2);

	return true;
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
	newWorldRotation = glm::normalize(newWorldRotation);
	SetWorldRotation(newWorldRotation);
}

void TransformComponent::SetWorldRotation(glm::quat quat)
{
	Entity* parent = owner->GetParent();
	if (parent) {
		rotation = glm::normalize(glm::inverse(parent->GetTransform().GetWorldRotation()) * quat);
	}
	else {
		rotation = glm::normalize(quat);
	}
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
		return glm::normalize(parent->GetTransform().GetWorldRotation() * rotation);
	}
	return glm::normalize(rotation);
}

glm::vec3 TransformComponent::GetWorldScale() const {
	Entity* parent = owner->GetParent();
	if (parent) {
		return parent->GetTransform().GetWorldScale() * scale;
	}
	return scale;
}

glm::vec3 TransformComponent::GetLocalPosition() const { return position;}

glm::quat TransformComponent::GetLocalRotation() const { return glm::normalize(rotation);}

glm::vec3 TransformComponent::GetLocalScale() const { return scale; }

glm::vec3 TransformComponent::GetForward() const {	return glm::normalize(rotation * glm::vec3(0.0f, 0.0f, 1.0f));}

glm::vec3 TransformComponent::GetUp() const {	return glm::normalize(rotation * glm::vec3(0.0f, 1.0f, 0.0f));}

glm::vec3 TransformComponent::GetRight() const {	return glm::normalize(rotation * glm::vec3(1.0f, 0.0f, 0.0f));}

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
