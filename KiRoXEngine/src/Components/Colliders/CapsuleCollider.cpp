#include "CapsuleCollider.h"

#include <Editor/Gizmos.h>

void CapsuleCollider::OnDrawGizmos()
{
	Gizmos::DrawCapsule(owner->GetTransform().GetWorldPosition(), 
						radius, 
						height, 
						owner->GetTransform().GetWorldRotation(), 
						glm::vec3(1.0f, 0.0f, 0.0f));
}
