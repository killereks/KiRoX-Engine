#include "BoxCollider.h"

#include "imgui.h"
#include "../Editor/Gizmos.h"

void BoxCollider::OnDrawGizmos()
{
	Gizmos::DrawWireCube(GetWorldPosition(), GetScale() * 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}

void BoxCollider::DrawInspector()
{
	ImGui::InputFloat3("Offset", &offset[0]);
	ImGui::InputFloat3("Scale", &scale[0]);
}
