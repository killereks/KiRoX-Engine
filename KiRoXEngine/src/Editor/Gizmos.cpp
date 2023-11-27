#include "Gizmos.h"

#include <glm/glm.hpp>
#include <gtc/constants.hpp>

void Gizmos::DrawWireCube(glm::vec3 position, glm::vec3 size, glm::vec3 color)
{

}

void Gizmos::DrawLine(glm::vec3 startPosition, glm::vec3 endPosition, glm::vec3 color)
{
	Gizmos* gizmos = GetInstance();

	gizmos->vertices.push_back(startPosition);
	gizmos->vertices.push_back(endPosition);

	gizmos->indices.push_back(0);
	gizmos->indices.push_back(1);
}

void Gizmos::DrawRay(glm::vec3 startPosition, glm::vec3 direction, glm::vec3 color)
{
	DrawLine(startPosition, startPosition + direction, color);
}

void Gizmos::DrawWireSphere(glm::vec3 pos, float radius, glm::vec3 color)
{

	/*float divisions = 16;

	for (int i = 0; i < divisions; i++)
	{
		float angle = 2.0f * glm::pi<float>() * i / divisions;

		float x = glm::cos(angle);
		float y = glm::sin(angle);

		GetInstance()->vertices.push_back(pos + glm::vec3(x, y, 0.0) * radius);
		GetInstance()->indices.push_back(i);
		GetInstance()->indices.push_back(i+1);
	}*/
}

Gizmos* Gizmos::instance = nullptr;