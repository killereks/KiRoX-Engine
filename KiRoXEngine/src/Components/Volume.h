#pragma once
#include "Component.h"
#include <Tools/ObjectPtr.h>
#include "icons/IconsFontAwesome6.h"

#include <Macros.h>

CLASS()
class Volume : public Component
{
	REFLECT(Component)

	PROPERTY()
	ObjectPtr* testProcess;

public:
	Volume();

	void Apply(unsigned int FBO);

	virtual std::string GetIcon() override {
		// 
		return ICON_FA_DISPLAY;
	}

};

