#include "Color.h"

Color::Color(glm::vec3 v)
{
	this->r = v.x;
	this->g = v.y;
	this->b = v.z;
	this->a = 1.0f;
}

Color::Color(glm::vec4 v)
{
	this->r = v.x;
	this->g = v.y;
	this->b = v.z;
	this->a = v.w;
}

Color::Color(float r, float g, float b, float a)
{
}

Color::Color()
{
	this->r = 1.0f;
	this->g = 1.0f;
	this->b = 1.0f;
}
