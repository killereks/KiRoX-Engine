#pragma once

#include <string>
#include <iostream>

#include <vector>
#include <glm/glm.hpp>

#include "Asset.h"

#include "fbx/ufbx.h"

class MeshFilter : public Asset {
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;

public:
	const std::vector<glm::vec3>& GetVertices() const { return vertices; }
	const std::vector<unsigned int>& GetIndices() const { return indices; }

	MeshFilter(const std::string& filePath)
	{
		vertices.clear();
		indices.clear();

		ufbx_load_opts opts = { 0 };
		ufbx_error error;
		ufbx_scene* scene = ufbx_load_file(filePath.c_str(), &opts, &error);

		if (!scene)
		{
			std::cout << "Failed to load file... " << error.description.data << "\n";
			return;
		}

		for (size_t i = 0; i < scene->nodes.count; i++)
		{
			ufbx_node* node = scene->nodes[i];
			if (node->is_root) continue;

			if (node->mesh)
			{
				for (size_t j = 0; j < node->mesh->num_vertices; j++)
				{
					auto vertex = node->mesh->vertices[j];
					vertices.emplace_back(vertex.x, vertex.y, vertex.z);
				}

				for (size_t j = 0; j < node->mesh->num_indices; j++)
				{
					uint32_t faceIndex = node->mesh->vertex_indices[j];
					indices.push_back(faceIndex);
				}
			}
		}

		ufbx_free_scene(scene);

		//aiMesh* mesh = scene->mMeshes[0];
		//
		//vertices.resize(mesh->mNumVertices);
		//uvs.resize(mesh->mNumVertices);
		//normals.resize(mesh->mNumVertices);
		//indices.resize(mesh->mNumFaces * 3);
		//
		//for (int i = 0; i < mesh->mNumVertices; i++)
		//{
		//	vertices[i] = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		//	if (mesh->HasNormals())
		//	{
		//		normals[i] = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		//	}
		//	if (mesh->HasTextureCoords(0))
		//	{
		//		uvs[i] = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		//	}
		//}
		//
		//for (int i = 0; i < mesh->mNumFaces; i++)
		//{
		//	aiFace face = mesh->mFaces[i];
		//	for (unsigned int j = 0; j < face.mNumIndices; j++)
		//	{
		//		indices.push_back(face.mIndices[j]);
		//	}
		//}
	}

};