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
	//ImGui::BeginTable("Mesh Information", 2);
	//
	//ImGui::TableNextColumn();
	//ImGui::Text("Vertices");
	//ImGui::TableNextColumn();
	//ImGui::Text("%d", vertices.size());
	//
	//ImGui::TableNextColumn();
	//ImGui::Text("Indices");
	//ImGui::TableNextColumn();
	//ImGui::Text("%d", indices.size());
	//
	//ImGui::TableNextColumn();
	//ImGui::Text("Normals");
	//ImGui::TableNextColumn();
	//ImGui::Text("%d", normals.size());
	//
	//ImGui::TableNextColumn();
	//ImGui::Text("UVs");
	//ImGui::TableNextColumn();
	//ImGui::Text("%d", uvs.size());
	//
	//ImGui::EndTable();
	
	ImGui::InputText("Mesh Name", &meshName);

	if (ImGui::Button("Apply"))
	{
		SetMeshFilter(meshName);
	}
}

void MeshComponent::Serialize(YAML::Emitter& out)
{
	std::cout << meshName << "\n";
	SERIALIZE_VALUE(meshName);
}

void MeshComponent::SetMeshFilter(std::string name)
{
	meshName = name;
	meshFilter = AssetManager::GetInstance()->Get<MeshFilter>(name);
}

Bounds& MeshComponent::GetBounds()
{
	Bounds& bounds = meshFilter->GetBounds();
	TransformComponent& transformComponent = owner->GetTransform();
	
	glm::vec3 min = glm::vec3(1.0f) * std::numeric_limits<float>::infinity();
	glm::vec3 max = glm::vec3(-1.0f) * std::numeric_limits<float>::infinity();

	std::vector<glm::vec3> boundVertices = bounds.GetVertices();

	glm::mat4 modelMatrix = owner->GetTransform().GetModelMatrix();

	for (int i = 0; i < boundVertices.size(); i++)
	{
		glm::vec3 vertex = boundVertices[i];
		glm::vec3 transformed = modelMatrix * glm::vec4(vertex.x, vertex.y, vertex.z, 1.0);

		min = glm::min(min, transformed);
		max = glm::max(max, transformed);
	}

	glm::vec3 pos = (max + min) * 0.5f;
	glm::vec3 size = (max - min) * 0.5f;

	delete boundsRotated;

	boundsRotated = new Bounds(pos, size);

	return *boundsRotated;
}

void MeshComponent::SimpleDraw(Shader* shader)
{
	shader->setMat4("modelMatrix", owner->GetTransform().GetModelMatrix());

	if (owner->GetName() == "Soldier")
	{
		Texture* tex = AssetManager::GetInstance()->Get<Texture>("bojovnikDiffuseMap.jpg");
		tex->Bind(1);
		shader->setInt("albedo", 1);
	}
	else if (owner->GetName() == "Castle")
	{
		Texture* tex = AssetManager::GetInstance()->Get<Texture>("Bretesche.jpg");
		tex->Bind(1);
		shader->setInt("albedo", 1);
	}

	if (meshFilter != nullptr)
	{
		meshFilter->DrawCall();
		
		//Bounds bounds = GetBounds();
		//
		//Gizmos::DrawWireCube(bounds.GetCenter(), bounds.GetSize(), glm::vec3(1.0));
	}
}
