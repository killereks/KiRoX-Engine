#include "TestComponent.h"
#include "imgui.h"
#include "icons/IconsFontAwesome6.h"

void TestComponent::DrawInspector()
{
	ImGui::Text("Your age is %s", age.c_str());
}

std::string TestComponent::GetIcon()
{
	return ICON_FA_FACE_GRIN;
}
