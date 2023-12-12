#pragma once

#include <glm/glm.hpp>
#include <vector>

class MeshSimplifier {
public:
	static void SimplifyMesh(std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices, float percentage);
};