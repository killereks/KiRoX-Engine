#pragma once

#include <string>
#include <iostream>

#include <vector>
#include <glm/glm.hpp>

#include "Asset.h"

#include "../Tools/File.h"

#include <thread>

#include "coroutines/minicoro.h"
#include "coroutines/CoroExtensions.h"

#include "../Tools/Bounds.h"
#include "../Tools/StatsCounter.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Tools/MeshSimplifier.h"

class MeshFilter : public Asset {

	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

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

	bool fullyUploaded = false;

	Bounds* bounds;

public:
	const std::vector<glm::vec3>& GetVertices() const { return vertices; }
	const std::vector<unsigned int>& GetIndices() const { return indices; }

	const std::vector<glm::vec2>& GetUVs() const { return uvs; }
	const std::vector<glm::vec3>& GetNormals() const { return normals; }

	size_t bufferUploadOffset = 0;
	size_t chunkSize = 16384;

	Bounds* GetBounds() const { 
		return bounds;
	}

	MeshFilter()
	{
		bounds = new Bounds();

		glGenBuffers(1, &VBO);
		glGenBuffers(1, &IBO);
		glGenBuffers(1, &NBO);
		glGenBuffers(1, &TBO);

		glGenVertexArrays(1, &VAO);
	}

	~MeshFilter()
	{
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &IBO);
		glDeleteBuffers(1, &NBO);
		glDeleteBuffers(1, &TBO);

		glDeleteVertexArrays(1, &VAO);

		delete bounds;
	}

	void UpdateBuffers()
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

	bool AsyncUpload()
	{
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		bool allDone = true;

		if (bufferUploadOffset < vertices.size())
		{
			size_t amount = std::min(chunkSize, vertices.size() - bufferUploadOffset);
			glBufferSubData(GL_ARRAY_BUFFER, bufferUploadOffset * sizeof(glm::vec3), amount * sizeof(glm::vec3), &vertices[bufferUploadOffset]);
			allDone = false;
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		if (bufferUploadOffset < indices.size())
		{
			size_t amount = std::min(chunkSize, indices.size() - bufferUploadOffset);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, bufferUploadOffset * sizeof(unsigned int), amount * sizeof(unsigned int), &indices[bufferUploadOffset]);
			allDone = false;
		}

		glBindBuffer(GL_ARRAY_BUFFER, NBO);
		if (bufferUploadOffset < normals.size())
		{
			size_t amount = std::min(chunkSize, normals.size() - bufferUploadOffset);
			glBufferSubData(GL_ARRAY_BUFFER, bufferUploadOffset * sizeof(glm::vec3), amount * sizeof(glm::vec3), &normals[bufferUploadOffset]);
			allDone = false;
		}

		glBindBuffer(GL_ARRAY_BUFFER, TBO);
		if (bufferUploadOffset < uvs.size())
		{
			size_t amount = std::min(chunkSize, uvs.size() - bufferUploadOffset);
			glBufferSubData(GL_ARRAY_BUFFER, bufferUploadOffset * sizeof(glm::vec2), amount * sizeof(glm::vec2), &uvs[bufferUploadOffset]);
			allDone = false;
		}

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		bufferUploadOffset += chunkSize;

		return allDone;
	}

	virtual co::Coro BeginLoading() override
	{
		co::Coro coro;

		coro.init([](){
			MeshFilter* self = (MeshFilter*)co::getUserData();

			bool hasLoaded = false;

			std::thread loadThread([self, &hasLoaded]()
				{
					self->LoadMesh();
					hasLoaded = true;
				});

			loadThread.detach();

			while (!hasLoaded)
			{
				co::yield();
			}

			self->UpdateBuffers();

			while (true)
			{
				if (self->AsyncUpload())
				{
					break;
				}
				co::yield();
			}

			self->fullyUploaded = true;

			}, this);

		return coro;
	}

	void DrawCall()
	{
		size_t size = std::min(bufferUploadOffset, indices.size());
		StatsCounter::GetInstance()->IncreaseCounter("triangles", size / 3);
		StatsCounter::GetInstance()->IncreaseCounter("drawCalls");

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, nullptr);
	}

	void LoadMesh()
	{
		vertices.clear();
		indices.clear();

		std::string fileExtension = filePath.substr(filePath.rfind("."));

		Assimp::Importer importer;
		unsigned int steps = aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_OptimizeMeshes |
			aiProcess_ImproveCacheLocality |
			aiProcess_FindDegenerates |
			aiProcess_FindInvalidData |
			aiProcess_OptimizeGraph |
			aiProcess_GenNormals;

		const aiScene* scene = importer.ReadFile(filePath, steps);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return;
		}

		aiMesh* mesh = scene->mMeshes[0];

		vertices.resize(mesh->mNumVertices);
		indices.resize(mesh->mNumFaces * 3);

		bounds->Clear();

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			glm::vec3 vertex = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

			bounds->InsertPoint(vertex);

			vertices[i] = vertex;

			if (mesh->HasNormals())
			{
				normals.emplace_back(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
			}
			if (mesh->HasTextureCoords(0))
			{
				uvs.push_back(glm::vec2(mesh->mTextureCoords[0][i].x, 1.0 - mesh->mTextureCoords[0][i].y));
			}
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		//if (indices.size() / 3 >= 25000) {
		//	MeshSimplifier::SimplifyMesh(vertices, indices, 0.75f);
		//}

		//if (fileName == "suzanne.fbx") {
		//	MeshSimplifier::SimplifyMesh(vertices, indices, 0.5f);
		//}
	}
};