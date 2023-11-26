#pragma once

#include "Component.h"

#include <vector>
#include <glm/glm.hpp>
#include "CameraComponent.h"
#include "../Assets/Shader.h"

class MeshComponent : public Component
{
	// BASICS:
	// Vertices
	// Indices
	// Normals
	// UVs
	
	// LATER:
	// tangents
	// bitangents

	unsigned int VBO; // vertices
	unsigned int IBO; // indices

	unsigned int VAO; // vertex array object

	// cached shader uniforms
	int modelMatrixLocation = -1;
	int viewMatrixLocation = -1;
	int projectionMatrixLocation = -1;

	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;

	public:
		MeshComponent();
		~MeshComponent();

		void DrawInspector() override;

		void SetQuad();

		void UpdateBuffers();

		void Draw(CameraComponent* cameraComponent, Shader* shader);
};

