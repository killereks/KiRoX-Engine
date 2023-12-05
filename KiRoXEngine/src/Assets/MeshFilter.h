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

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class MeshFilter : public Asset {
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

public:
	const std::vector<glm::vec3>& GetVertices() const { return vertices; }
	const std::vector<unsigned int>& GetIndices() const { return indices; }

	const std::vector<glm::vec2>& GetUVs() const { return uvs; }
	const std::vector<glm::vec3>& GetNormals() const { return normals; }

	MeshFilter()
	{

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

			}, this);

		return coro;
	}

	void LoadMesh()
	{
		vertices.clear();
		indices.clear();

		std::cout << "Loading mesh " << std::this_thread::get_id() << "\n";

		std::string fileExtension = filePath.substr(filePath.rfind("."));

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return;
		}

		aiMesh* mesh = scene->mMeshes[0];

		vertices.resize(mesh->mNumVertices);
		indices.resize(mesh->mNumFaces * 3);
		//normals.resize(mesh->mNumVertices);
		//uvs.resize(mesh->mNumVertices);

		for (int i = 0; i < mesh->mNumVertices; i++)
		{
			vertices[i] = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			if (mesh->HasNormals())
			{
				normals.emplace_back(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
			}
			if (mesh->HasTextureCoords(0))
			{
				//uvs[i] = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
				uvs.push_back(glm::vec2(mesh->mTextureCoords[0][i].x, 1.0 - mesh->mTextureCoords[0][i].y));
			}
		}

		for (int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		if (normals.size() == 0)
		{
			// calculate them

			for (int i = 0; i < indices.size(); i+=3)
			{
				int face0 = indices[i + 0];
				int face1 = indices[i + 1];
				int face2 = indices[i + 2];

				glm::vec3 v0 = vertices[face0];
				glm::vec3 v1 = vertices[face1];
				glm::vec3 v2 = vertices[face2];

				glm::vec3 dir0 = v1 - v0;
				glm::vec3 dir1 = v2 - v0;

				glm::vec3 normal = glm::cross(dir0, dir1);

				normals.push_back(normal);
			}
		}
	}

};