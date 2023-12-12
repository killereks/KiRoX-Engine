#include "MeshSimplifier.h"
#include <iostream>
#include <execution>

#include "mesh_simp/Simplify.h"

void MeshSimplifier::SimplifyMesh(std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices, float percentage) {
	assert(percentage > 0.0f && percentage < 1.0f);

	Simplify::vertices.clear();
	Simplify::triangles.clear();

	for (const auto& vertex : vertices) {
		Simplify::Vertex v;
		v.p.x = vertex.x;
		v.p.y = vertex.y;
		v.p.z = vertex.z;

		Simplify::vertices.push_back(v);
	}

	for (size_t i = 0; i < indices.size(); i += 3) {
		Simplify::Triangle t;
		t.v[0] = indices[i];
		t.v[1] = indices[i + 1];
		t.v[2] = indices[i + 2];

		Simplify::triangles.push_back(t);
	}

	size_t targetCount = static_cast<size_t>(percentage * indices.size() / 3);

	Simplify::simplify_mesh(targetCount, 5.0, true);

	vertices.clear();
	indices.clear();

	for (const auto& vertex : Simplify::vertices) {
		vertices.emplace_back(vertex.p.x, vertex.p.y, vertex.p.z);
	}

	for (const auto& triangle : Simplify::triangles) {
		indices.push_back(triangle.v[0]);
		indices.push_back(triangle.v[1]);
		indices.push_back(triangle.v[2]);
	}
}