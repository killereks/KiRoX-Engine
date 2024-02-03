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

	PROPERTY()
	float a;

public:
	Color(glm::vec3 v);
	Color(glm::vec4 v);
	Color(float r, float g, float b, float a = 1.0f);
	Color();

	inline glm::vec4 GetAsVector() {
		return glm::vec4(r, g, b, a);
	}

	inline void SetFromVector(glm::vec3 v) {
		r = v.x;
		g = v.y;
		b = v.z;
	}

	inline void SetFromVector(glm::vec4 v) {
		r = v.x;
		g = v.y;
		b = v.z;
		a = v.w;
	}

};

