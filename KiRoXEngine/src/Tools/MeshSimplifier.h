#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "mesh_simp/mdMeshDecimator.h"

class MeshSimplifier {
public:
	static void SimplifyMesh(std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices, float percentage);
};