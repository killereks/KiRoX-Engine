#include "MeshComponent.h"

#include <GL/glew.h>
#include "imgui.h"

#include "imgui_stdlib.h"

#include "../Assets/AssetManager.h"
#include "../Assets/MeshFilter.h"

MeshComponent::MeshComponent()
{
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);
	glGenBuffers(1, &NBO);
	glGenBuffers(1, &TBO);

	glGenVertexArrays(1, &VAO);
}

MeshComponent::~MeshComponent()
{
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
	glDeleteBuffers(1, &NBO);
	glDeleteBuffers(1, &TBO);

	glDeleteVertexArrays(1, &VAO);
}

void MeshComponent::DrawInspector()
{
	ImGui::BeginTable("Mesh Information", 2);

	ImGui::TableNextColumn();
	ImGui::Text("Vertices");
	ImGui::TableNextColumn();
	ImGui::Text("%d", vertices.size());

	ImGui::TableNextColumn();
	ImGui::Text("Indices");
	ImGui::TableNextColumn();
	ImGui::Text("%d", indices.size());

	ImGui::TableNextColumn();
	ImGui::Text("Normals");
	ImGui::TableNextColumn();
	ImGui::Text("%d", normals.size());

	ImGui::TableNextColumn();
	ImGui::Text("UVs");
	ImGui::TableNextColumn();
	ImGui::Text("%d", uvs.size());

	ImGui::EndTable();
	
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

	AssetManager::GetInstance()->AddCallback_OnLoaded(name, [this]()
		{
			OnLoaded();
		});
}

void MeshComponent::OnLoaded()
{
	MeshFilter* meshFilter = AssetManager::GetInstance()->Get<MeshFilter>(meshName);

	bufferUploadOffset = 0;

	vertices = meshFilter->GetVertices();
	indices = meshFilter->GetIndices();
	normals = meshFilter->GetNormals();
	uvs = meshFilter->GetUVs();

	UpdateBuffers();
}

void MeshComponent::AsyncUpload()
{
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	if (bufferUploadOffset < vertices.size())
	{
		std::cout << "Uploading verts offset: " << bufferUploadOffset << "\n";
		size_t amount = std::min(chunkSize, vertices.size() - bufferUploadOffset);
		glBufferSubData(GL_ARRAY_BUFFER, bufferUploadOffset * sizeof(glm::vec3), amount * sizeof(glm::vec3), &vertices[bufferUploadOffset]);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	if (bufferUploadOffset < indices.size())
	{
		size_t amount = std::min(chunkSize, indices.size() - bufferUploadOffset);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, bufferUploadOffset * sizeof(unsigned int), amount * sizeof(unsigned int), &indices[bufferUploadOffset]);
	}

	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	if (bufferUploadOffset < normals.size())
	{
		size_t amount = std::min(chunkSize, normals.size() - bufferUploadOffset);
		glBufferSubData(GL_ARRAY_BUFFER, bufferUploadOffset * sizeof(glm::vec3), amount * sizeof(glm::vec3), &normals[bufferUploadOffset]);
	}

	glBindBuffer(GL_ARRAY_BUFFER, TBO);
	if (bufferUploadOffset < uvs.size())
	{
		size_t amount = std::min(chunkSize, uvs.size() - bufferUploadOffset);
		glBufferSubData(GL_ARRAY_BUFFER, bufferUploadOffset * sizeof(glm::vec2), amount * sizeof(glm::vec2), &uvs[bufferUploadOffset]);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	bufferUploadOffset += chunkSize;
}

void MeshComponent::SetQuad()
{
	vertices = {
		glm::vec3(-0.5f, -0.5f, 0.0f),
		glm::vec3( 0.5f, -0.5f, 0.0f),
		glm::vec3( 0.5f,  0.5f, 0.0f),
		glm::vec3(-0.5f,  0.5f, 0.0f)
	};

	indices = {
		2, 1, 0,
		0, 3, 2
	};

	UpdateBuffers();
}

void MeshComponent::UpdateBuffers()
{
	glBindVertexArray(VAO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), nullptr, GL_STATIC_DRAW);

	// VERTICES
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), nullptr, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// NORMALS
	if (normals.size() > 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, NBO);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), nullptr, GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	// UVs
	if (uvs.size() > 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, TBO);
		glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), nullptr, GL_STATIC_DRAW);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void MeshComponent::SimpleDraw(Shader* shader)
{
	AsyncUpload();
	shader->setMat4("modelMatrix", owner->GetTransform().GetModelMatrix());

	size_t size = std::min(bufferUploadOffset, indices.size());

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

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, nullptr);
}
