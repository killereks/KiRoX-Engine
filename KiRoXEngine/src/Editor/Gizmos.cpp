#include "Gizmos.h"

#include <glm/glm.hpp>
#include <gtc/constants.hpp>
#include "../Math/Mathf.h"
#include "GLFW/glfw3.h"

void Gizmos::DrawWireCube(glm::vec3 position, glm::vec3 size, glm::vec3 color)
{
	Gizmos* gizmos = GetInstance();

	glm::vec3 halfSize = size * 0.5f;

	// Define the 8 vertices of the cube
	glm::vec3 vertices[8] = {
		position + glm::vec3(-halfSize.x, -halfSize.y, -halfSize.z),
		position + glm::vec3(halfSize.x, -halfSize.y, -halfSize.z),
		position + glm::vec3(halfSize.x, halfSize.y, -halfSize.z),
		position + glm::vec3(-halfSize.x, halfSize.y, -halfSize.z),
		position + glm::vec3(-halfSize.x, -halfSize.y, halfSize.z),
		position + glm::vec3(halfSize.x, -halfSize.y, halfSize.z),
		position + glm::vec3(halfSize.x, halfSize.y, halfSize.z),
		position + glm::vec3(-halfSize.x, halfSize.y, halfSize.z)
	};

	// Define the 12 edges of the cube
	int edges[12][2] = {
		{0, 1}, {1, 2}, {2, 3}, {3, 0},  // Bottom face
		{4, 5}, {5, 6}, {6, 7}, {7, 4},  // Top face
		{0, 4}, {1, 5}, {2, 6}, {3, 7}   // Connecting edges
	};

	// Draw each edge
	for (int i = 0; i < 12; ++i)
	{
		DrawLine(vertices[edges[i][0]], vertices[edges[i][1]], color);
	}
}

void Gizmos::DrawWireCubeRotated(glm::vec3 position, glm::vec3 size, glm::quat rotation, glm::vec3 color)
{
	Gizmos* gizmos = GetInstance();

	glm::vec3 halfSize = size * 0.5f;

	// Define the 8 vertices of the cube
	glm::vec3 vertices[8] = {
		position + rotation * glm::vec3(-halfSize.x, -halfSize.y, -halfSize.z),
		position + rotation * glm::vec3(halfSize.x, -halfSize.y, -halfSize.z),
		position + rotation * glm::vec3(halfSize.x, halfSize.y, -halfSize.z),
		position + rotation * glm::vec3(-halfSize.x, halfSize.y, -halfSize.z),
		position + rotation * glm::vec3(-halfSize.x, -halfSize.y, halfSize.z),
		position + rotation * glm::vec3(halfSize.x, -halfSize.y, halfSize.z),
		position + rotation * glm::vec3(halfSize.x, halfSize.y, halfSize.z),
		position + rotation * glm::vec3(-halfSize.x, halfSize.y, halfSize.z)
	};

	// Define the 12 edges of the cube
	int edges[12][2] = {
		{0, 1}, {1, 2}, {2, 3}, {3, 0},  // Bottom face
		{4, 5}, {5, 6}, {6, 7}, {7, 4},  // Top face
		{0, 4}, {1, 5}, {2, 6}, {3, 7}   // Connecting edges
	};

	// Draw each edge
	for (int i = 0; i < 12; ++i)
	{
		DrawLine(vertices[edges[i][0]], vertices[edges[i][1]], color);
	}
}

void Gizmos::DrawLine(glm::vec3 startPosition, glm::vec3 endPosition, glm::vec3 color)
{
	Gizmos* gizmos = GetInstance();

	unsigned int startIndex = gizmos->vertices.size();

	gizmos->vertices.push_back(startPosition);
	gizmos->vertices.push_back(endPosition);

	gizmos->indices.push_back(startIndex);
	gizmos->indices.push_back(startIndex+1);

	gizmos->colors.push_back(color);
	gizmos->colors.push_back(color);
}

void Gizmos::DrawRay(glm::vec3 startPosition, glm::vec3 direction, glm::vec3 color)
{
	DrawLine(startPosition, startPosition + direction, color);
}

void Gizmos::DrawWireSphere(glm::vec3 pos, float radius, glm::vec3 color)
{
	int divisions = 16;

	constexpr float tau = glm::pi<float>() * 2.0f;

	std::vector<glm::vec3> points;

	for (int i = 0; i <= divisions; i++)
	{
		float perc = ((float)i + 0.5f) / (float)divisions;

		float angle = Mathf::Lerp(0.0f, tau, perc);

		glm::vec3 point = pos;
		point.x += glm::cos(angle) * radius;
		point.z += glm::sin(angle) * radius;

		points.push_back(point);
	}

	for (int i = 0; i < points.size() - 1; ++i)
	{
		DrawLine(points[i], points[i + 1], color);
	}

	DrawLine(points[points.size() - 1], points[0], color);

	points.clear();

	for (int i = 0; i <= divisions; i++)
	{
		float perc = ((float)i + 0.5f) / (float)divisions;

		float angle = Mathf::Lerp(0.0f, tau, perc);

		glm::vec3 point = pos;
		point.x += glm::cos(angle) * radius;
		point.y += glm::sin(angle) * radius;

		points.push_back(point);
	}

	for (int i = 0; i < points.size() - 1; ++i)
	{
		DrawLine(points[i], points[i + 1], color);
	}
	DrawLine(points[points.size() - 1], points[0], color);

	points.clear();

	for (int i = 0; i <= divisions; i++)
	{
		float perc = ((float)i + 0.5f) / (float)divisions;

		float angle = Mathf::Lerp(0.0f, tau, perc);

		glm::vec3 point = pos;
		point.y += glm::cos(angle) * radius;
		point.z += glm::sin(angle) * radius;

		points.push_back(point);
	}

	for (int i = 0; i < points.size() - 1; ++i)
	{
		DrawLine(points[i], points[i + 1], color);
	}
	DrawLine(points[points.size() - 1], points[0], color);

	points.clear();
}


Gizmos* Gizmos::instance = nullptr;