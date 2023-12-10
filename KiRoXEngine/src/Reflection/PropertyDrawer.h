#pragma once

#include <rttr/registration>

class PropertyDrawer
{
public:
	static void DrawProperty(rttr::property& prop, rttr::variant& objInstance);

	static void DrawSingleProperty(rttr::property& prop, rttr::variant& objInstance);

	static void DrawFunction(rttr::method& method, rttr::variant& objInstance);
};

