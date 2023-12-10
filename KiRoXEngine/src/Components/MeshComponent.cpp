#include "MeshComponent.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "imgui.h"

#include "imgui_stdlib.h"

#include "../Assets/AssetManager.h"
#include "../Assets/MeshFilter.h"
#include "../Editor/Gizmos.h"
#include "../Tools/StatsCounter.h"

MeshComponent::MeshComponent()
{
	
}

MeshComponent::~MeshComponent()
{
	
}

void MeshComponent::DrawInspector()
{	
	ImGui::PushItemWidth(-1);
	if (meshFilter == nullptr)
	{
		ImGui::Text("No mesh asset...");
	}
	else
	{
		ImGui::Text(meshFilter->fileName.c_str());
	}
	ImGui::PopItemWidth();
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET"))
		{
			MeshFilter* draggedMeshFilter = dynamic_cast<MeshFilter*>(*(Asset**)payload->Data);

			if (draggedMeshFilter != nullptr)
			{
				SetMeshUUID(draggedMeshFilter->uuid);
			}
		}
		ImGui::EndDragDropTarget();
	}
}

void MeshComponent::SetMeshUUID(std::string uuid)
{
	meshUUID = uuid;

	if (meshUUID != "")
	{
		meshFilter = AssetManager::GetInstance()->GetByUUID<MeshFilter>(meshUUID);
	}
	else
	{
		meshFilter = nullptr;
	}
}

void MeshComponent::Serialize(YAML::Emitter& out)
{
	SERIALIZE_VALUE(meshUUID);
}

Bounds* MeshComponent::GetBounds()
{
	if (meshFilter == nullptr)
	{
		return nullptr;
	}

	Bounds* bounds = meshFilter->GetBounds();
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
	
	//glm::vec3 min = glm::vec3(1.0f) * std::numeric_limits<float>::infinity();
	//glm::vec3 max = glm::vec3(-1.0f) * std::numeric_limits<float>::infinity();
	//
	//std::vector<glm::vec3> boundVertices = bounds.GetVertices();
	//
	//glm::mat4 modelMatrix = owner->GetTransform().GetModelMatrix();
	//
	//for (int i = 0; i < boundVertices.size(); i++)
	//{
	//	glm::vec3 vertex = boundVertices[i];
	//	glm::vec3 transformed = modelMatrix * glm::vec4(vertex.x, vertex.y, vertex.z, 1.0);
	//
	//	Gizmos::DrawWireSphere(vertex, 0.5f, glm::vec3(1.0, 0.0, 0.0));
	//	Gizmos::DrawWireSphere(transformed, 0.5f, glm::vec3(0.0, 1.0, 0.0));
	//	Gizmos::DrawLine(vertex, transformed, glm::vec3(0.0, 0.0, 1.0));
	//
	//	min = glm::min(min, transformed);
	//	max = glm::max(max, transformed);
	//}
	//
	//glm::vec3 pos = (max + min) * 0.5f;
	//glm::vec3 size = (max - min) * 0.5f;
	//
	//delete boundsRotated;
	//
	//boundsRotated = new Bounds(pos, size);
	//
	//return *boundsRotated;
}

void MeshComponent::SimpleDraw(Shader* shader)
{
	shader->setMat4("modelMatrix", owner->GetTransform().GetModelMatrix());

	Texture* tex = AssetManager::GetInstance()->Get<Texture>("bojovnikDiffuseMap.jpg");
	tex->Bind(1);
	shader->setInt("albedo", 1);

	if (owner->GetName() == "Castle")
	{
		Texture* tex = AssetManager::GetInstance()->Get<Texture>("Bretesche.jpg");
		tex->Bind(1);
		shader->setInt("albedo", 1);
	}

	if (meshFilter != nullptr)
	{
		meshFilter->DrawCall();
		
		Bounds* bounds = GetBounds();
		if (bounds != nullptr)
		{
			Gizmos::DrawWireCube(bounds->GetCenter(), bounds->GetSize(), glm::vec3(1.0));
		}
	}
}
