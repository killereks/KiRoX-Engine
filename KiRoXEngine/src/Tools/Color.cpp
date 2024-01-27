#include "Color.h"

Color::Color(glm::vec3 v)
{
	this->r = v.x;
	this->g = v.y;
	this->b = v.z;
}

Color::Color(float r, float g, float b)
{
	this->r = r;
	this->g = g;
	this->b = b;
}

Color::Color()
{
	this->r = 1.0f;
	this->g = 1.0f;
	this->b = 1.0f;
}
