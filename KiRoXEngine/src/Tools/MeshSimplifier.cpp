#include "MeshSimplifier.h"
#include <iostream>
#include <execution>

using namespace MeshDecimation;

void MeshSimplifier::SimplifyMesh(std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices, float percentage) {
	assert(percentage > 0.0f && percentage < 1.0f);

	size_t targetTriangles = indices.size() * percentage;
	size_t targetVertices = vertices.size() * percentage;

	MeshDecimator decimator;
	Vec3<Float>* verts;
	Vec3<int>* indx;

	verts = new Vec3<Float>[vertices.size()];
	indx = new Vec3<int>[indices.size()];

	for (int i = 0; i < vertices.size(); i++) {
		verts[i] = Vec3<MeshDecimation::Float>(vertices[i].x, vertices[i].y, vertices[i].z);
	}

	for (int i = 0; i < indices.size(); i++) {
		indx[i] = Vec3<int>(indices[i], indices[i + 1], indices[i + 2]);
	}

	decimator.Initialize(vertices.size(), indices.size(), &verts[0], &indx[0]);
	decimator.Decimate(targetVertices, targetTriangles, 1.0f);

	std::vector<Vec3<Float>> decimatedVertices;
	std::vector<Vec3<int>> decimatedIndices;

	decimatedVertices.resize(decimator.GetNVertices());
	decimatedIndices.resize(decimator.GetNTriangles());

	decimator.GetMeshData(&decimatedVertices[0], &decimatedIndices[0]);

	vertices.clear();
	indices.clear();

	vertices.resize(decimatedVertices.size());
	indices.resize(decimatedIndices.size());

	for (int i = 0; i < decimatedVertices.size(); i++) {
		vertices[i] = glm::vec3(decimatedVertices[i].X(), decimatedVertices[i].Y(), decimatedVertices[i].Z());
	}

	for (int i = 0; i < decimatedIndices.size(); i++) {
		indices[i] = decimatedIndices[i].X();
		indices[i + 1] = decimatedIndices[i].Y();
		indices[i + 2] = decimatedIndices[i].Z();
	}

	delete[] verts;
	delete[] indx;
}