#pragma once
#include "Component.h"
#include "../Macros.h"

CLASS()
class TestComponent : public Component
{
	REFLECT(Component);

	PROPERTY()
	std::string age;

	virtual void DrawInspector() override;

	virtual std::string GetIcon() override;
};

