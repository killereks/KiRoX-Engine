#pragma once

#include <ext/vector_float3.hpp>
#include <vector>

class Bounds
{
	public:
	Bounds();
	Bounds(glm::vec3 pos, glm::vec3 halfSize);
	~Bounds();

	void Clear();

	void InsertPoint(glm::vec3 point);

	const glm::vec3 GetCenter();
	const glm::vec3 GetSize();

	const glm::vec3 GetMin();
	const glm::vec3 GetMax();

	std::vector<glm::vec3> GetVertices();

	glm::vec3 ClosestPoint(glm::vec3 point);
	bool Intersects(Bounds bounds);
	bool Contains(glm::vec3 point);

	void GetRenderCube(std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices);

 private:
	glm::vec3 min;
	glm::vec3 max;
	glm::vec3 center;
	glm::vec3 size;

	std::vector<glm::vec3> m_Points;
};

