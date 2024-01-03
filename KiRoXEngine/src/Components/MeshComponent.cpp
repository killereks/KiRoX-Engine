#include "MeshComponent.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "imgui.h"

#include "imgui_stdlib.h"

#include "../Assets/AssetManager.h"
#include "../Assets/MeshFilter.h"
#include "../Editor/Gizmos.h"
#include "../Tools/StatsCounter.h"

#include "../Reflection/PropertyDrawer.h"

MeshComponent::MeshComponent()
{
	mesh = new ObjectPtr();
	material = new ObjectPtr();
}

MeshComponent::~MeshComponent()
{
	
}

Bounds* MeshComponent::GetBounds()
{
	if (GetMeshFilter() == nullptr)
	{
		return nullptr;
	}

	Bounds* bounds = GetMeshFilter()->GetBounds();
	TransformComponent& transform = owner->GetTransform();

	const glm::vec3 globalCenter = transform.GetModelMatrix() * glm::vec4(bounds->GetCenter(), 1.0f);

	glm::vec3 extents = bounds->GetSize() * transform.GetWorldScale();

	const glm::vec3 right = transform.GetRight() * extents.x;
	const glm::vec3 up = transform.GetUp() * extents.y;
	const glm::vec3 forward = transform.GetForward() * extents.z;

	const float Ii = std::abs(glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), right)) +
		std::abs(glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), up)) +
		std::abs(glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), forward));

	const float Ij = std::abs(glm::dot(glm::vec3(0.0f, 1.0f, 0.0f), right)) +
		std::abs(glm::dot(glm::vec3(0.0f, 1.0f, 0.0f), up)) +
		std::abs(glm::dot(glm::vec3(0.0f, 1.0f, 0.0f), forward));

	const float Ik = std::abs(glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), right)) +
		std::abs(glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), up)) +
		std::abs(glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), forward));

	boundsRotated = new Bounds(globalCenter, glm::vec3(Ii, Ij, Ik) * 0.5f);

	return boundsRotated;
}

void MeshComponent::SimpleDraw(Shader* shader)
{
	shader->setMat4("modelMatrix", owner->GetTransform().GetModelMatrix());

	if (GetMeshFilter() != nullptr)
	{
		//Bounds* bounds = GetBounds();
		//if (bounds != nullptr)
		//{
		//	//Gizmos::DrawWireCube(bounds->GetCenter(), bounds->GetSize(), glm::vec3(1.0));
		//
		//	//shader->setVec3("boundingBoxMin", bounds->GetMin());
		//	//shader->setVec3("boundingBoxMax", bounds->GetMax());
		//}

		GetMeshFilter()->DrawCall();
	}
}
