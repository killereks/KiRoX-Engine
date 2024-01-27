#pragma once

#include <glm/glm.hpp>
#include <Macros.h>

CLASS()
class Color {
	REFLECT()

	PROPERTY()
	float r;

	PROPERTY()
	float g;

	PROPERTY()
	float b;

public:
	Color(glm::vec3 v);
	Color(float r, float g, float b);
	Color();

	inline glm::vec3 GetAsVector() {
		return glm::vec3(r, g, b);
	}

	inline void SetFromVector(glm::vec3 v) {
		r = v.x;
		g = v.y;
		b = v.z;
	}

};

