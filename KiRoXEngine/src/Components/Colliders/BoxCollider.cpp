#include "BoxCollider.h"

#include "imgui.h"
#include <Editor/Gizmos.h>

BoxCollider::BoxCollider()
{
	offset = glm::vec3(0.0f);
	scale = glm::vec3(1.0f);
}

void BoxCollider::OnDrawGizmos()
{
	Gizmos::DrawWireCubeRotated(GetWorldPosition(), GetScale() * 2.0f, GetWorldRotation(), glm::vec3(0.0f, 1.0f, 0.0f));
}

bool BoxCollider::DrawInspector()
{
	ImGui::InputFloat3("Offset", &offset[0]);
	ImGui::InputFloat3("Scale", &scale[0]);

	return true;
}
