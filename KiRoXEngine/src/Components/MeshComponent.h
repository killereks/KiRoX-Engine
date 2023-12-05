#pragma once

#include "Component.h"

#include <vector>
#include <glm/glm.hpp>
#include "../Assets/Shader.h"
#include "../Macros.h"
#include "../Entity.h"
#include "icons/IconsFontAwesome6.h"

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

	unsigned int NBO; // normals
	unsigned int TBO; // texture coords

	unsigned int VAO; // vertex array object

	// cached shader uniforms
	int modelMatrixLocation = -1;
	int viewMatrixLocation = -1;
	int projectionMatrixLocation = -1;

	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;

	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;

	std::string meshName;

	size_t bufferUploadOffset = 0;
	size_t chunkSize = 1024;

	public:
		MeshComponent();
		~MeshComponent();

		void DrawInspector() override;
		void Serialize(YAML::Emitter& out) override;

		void SetMeshFilter(std::string name);
		void OnLoaded();

		void AsyncUpload();

		std::string GetIcon() override
		{
			return " " ICON_FA_CUBE;
		}

		void SetQuad();

		void UpdateBuffers();

		void SimpleDraw(Shader* shader);
};

