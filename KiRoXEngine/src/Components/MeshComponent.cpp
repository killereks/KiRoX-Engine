#include "MeshComponent.h"

#include <GL/glew.h>
#include "imgui.h"

MeshComponent::MeshComponent()
{
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);

	glGenVertexArrays(1, &VAO);

	SetQuad();
}

MeshComponent::~MeshComponent()
{
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);

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

	ImGui::EndTable();
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

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(0);
}

void MeshComponent::Draw(CameraComponent* cameraComponent, Shader* shader)
{
	shader->use();

	if (modelMatrixLocation == -1) {
		modelMatrixLocation = glGetUniformLocation(shader->ID, "modelMatrix");
	}

	if (viewMatrixLocation == -1) {
		viewMatrixLocation = glGetUniformLocation(shader->ID, "viewMatrix");
	}

	if (projectionMatrixLocation == -1) {
		projectionMatrixLocation = glGetUniformLocation(shader->ID, "perspectiveMatrix");
	}

	shader->setMat4(modelMatrixLocation, owner->GetTransform().GetModelMatrix());
	shader->setMat4(viewMatrixLocation, cameraComponent->GetViewMatrix());
	shader->setMat4(projectionMatrixLocation, cameraComponent->GetProjectionMatrix());

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
}
