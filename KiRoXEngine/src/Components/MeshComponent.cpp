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
	
}

MeshComponent::~MeshComponent()
{
	
}

bool MeshComponent::DrawInspector()
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

	//PropertyDrawer::DrawAssetDragDrop<Shader>(shader);

	if (ImGui::BeginTable("Mesh Info", 2, ImGuiTableFlags_Hideable | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable)) {
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Text("Vertex Count:");
		ImGui::TableNextColumn();
		ImGui::Text("%d", GetVertexCount());

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Text("Triangle Count:");
		ImGui::TableNextColumn();
		ImGui::Text("%d", GetTriangleCount());
		ImGui::EndTable();
	}

	return true;
}

void MeshComponent::SetMeshUUID(std::string uuid)
{
	meshUUID = uuid;
	UpdateUUID();
}

void MeshComponent::UpdateUUID()
{
	if (meshUUID != "")
	{
		meshFilter = AssetManager::GetInstance()->GetByUUID<MeshFilter>(meshUUID);
	}
	else
	{
		meshFilter = nullptr;
	}
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
}

void MeshComponent::SimpleDraw(Shader* shader)
{
	shader->setMat4("modelMatrix", owner->GetTransform().GetModelMatrix());

	if (meshFilter != nullptr)
	{
		//Bounds* bounds = GetBounds();
		//if (bounds != nullptr)
		//{
		//	//Gizmos::DrawWireCube(bounds->GetCenter(), bounds->GetSize(), glm::vec3(1.0));
		//
		//	//shader->setVec3("boundingBoxMin", bounds->GetMin());
		//	//shader->setVec3("boundingBoxMax", bounds->GetMax());
		//}

		meshFilter->DrawCall();
	}
	else {
		UpdateUUID();
	}
}
