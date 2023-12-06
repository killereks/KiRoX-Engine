#include "Bounds.h"
#include <common.hpp>
#include <ext/quaternion_geometric.hpp>

Bounds::Bounds()
{

}

Bounds::Bounds(glm::vec3 pos, glm::vec3 halfSize)
{
	min = pos - halfSize;
	max = pos + halfSize;
	center = pos;
	size = halfSize * 2.0f;
}

Bounds::~Bounds()
{
	m_Points.clear();
}

void Bounds::Clear()
{
	m_Points.clear();
}

void Bounds::InsertPoint(glm::vec3 point)
{
	m_Points.push_back(point);

	min.x = glm::min(min.x, point.x);
	min.y = glm::min(min.y, point.y);
	min.z = glm::min(min.z, point.z);

	max.x = glm::max(max.x, point.x);
	max.y = glm::max(max.y, point.y);
	max.z = glm::max(max.z, point.z);

	center = (min + max) / 2.0f;
	size = max - min;
}

const glm::vec3 Bounds::GetCenter()
{
	return center;
}

const glm::vec3 Bounds::GetSize()
{
	return size;
}

const glm::vec3 Bounds::GetMin()
{
	return min;
}

const glm::vec3 Bounds::GetMax()
{
	return max;
}


std::vector<glm::vec3> Bounds::GetVertices()
{
	std::vector<glm::vec3> outVertices;

	outVertices.emplace_back(min.x, min.y, min.z);
	outVertices.emplace_back(min.x, min.y, max.z);
	outVertices.emplace_back(min.x, max.y, min.z);
	outVertices.emplace_back(max.x, min.y, min.z);
	outVertices.emplace_back(min.x, max.y, max.z);
	outVertices.emplace_back(max.x, min.y, max.z);
	outVertices.emplace_back(max.x, max.y, min.z);
	outVertices.emplace_back(max.x, max.y, max.z);

	return outVertices;
}

glm::vec3 Bounds::ClosestPoint(glm::vec3 point)
{
	glm::vec3 result;

	result.x = glm::clamp(point.x, min.x, max.x);
	result.y = glm::clamp(point.y, min.y, max.y);
	result.z = glm::clamp(point.z, min.z, max.z);

	return result;
}

bool Bounds::Intersects(Bounds bounds)
{
	if (bounds.min.x > max.x) return false;
	if (bounds.min.y > max.y) return false;
	if (bounds.min.z > max.z) return false;

	if (bounds.max.x < min.x) return false;
	if (bounds.max.y < min.y) return false;
	if (bounds.max.z < min.z) return false;

	return true;
}

bool Bounds::Contains(glm::vec3 point)
{
	if (point.x < min.x) return false;
	if (point.y < min.y) return false;
	if (point.z < min.z) return false;

	if (point.x > max.x) return false;
	if (point.y > max.y) return false;
	if (point.z > max.z) return false;

	return true;
}