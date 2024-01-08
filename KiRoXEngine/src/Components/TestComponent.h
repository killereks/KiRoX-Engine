#pragma once
#include "Component.h"
#include "../Macros.h"

CLASS()
class TestComponent : public Component
{
	REFLECT(Component);

	PROPERTY()
	int a;

	PROPERTY()
	float time;

	PROPERTY()
	std::string name;

	PROPERTY()
	std::vector<int> numbers;

	virtual std::string GetIcon() override;
};

