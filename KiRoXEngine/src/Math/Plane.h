#pragma once

#include <glm/glm.hpp>

struct Plane {
	glm::vec3 normal;

	// from origin to nearest point in the plane
	float distance;

	Plane() = default;

	Plane(const glm::vec3& point, const glm::vec3& norm) : normal(glm::normalize(norm)), distance(glm::dot(normal, point)) {

	}

	float GetSignedDistanceToPlane(const glm::vec3& point) const {
		return glm::dot(normal, point) - distance;
	}
};