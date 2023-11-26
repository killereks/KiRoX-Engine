#include "CameraComponent.h"

#include "imgui.h"

CameraComponent::CameraComponent()
{
	nearClipPlane = 0.3f;
	farClipPlane = 1000.0f;
	fieldOfView = 60.0f;

	cameraType = CameraType::Perspective;

	left = -10.0f;
	right = 10.0f;
	top = 10.0f;
	bottom = -10.0f;
	orthoNear = 1.0f;
	orthoFar = 100.0f;
}

CameraComponent::~CameraComponent()
{
}

void CameraComponent::DrawInspector()
{
	if (cameraType == CameraType::Perspective) {
		ImGui::Text("Perspective Camera");
		ImGui::SliderFloat("Near Clip Plane", &nearClipPlane, 0.1f, 100.0f);
		ImGui::SliderFloat("Far Clip Plane", &farClipPlane, 0.1f, 100.0f);

		ImGui::SliderFloat("Field Of View", &fieldOfView, 1.0f, 179.0f, "%.0f");

		if (ImGui::Button("Change to Orthographic", ImVec2(-1, 0))) {
			cameraType = CameraType::Orthographic;
		}
	}
	else if (cameraType == CameraType::Orthographic) {
		ImGui::Text("Orthographic Camera");

		ImGui::DragFloat("Left", &left, -100.0f, 100.0f);
		ImGui::DragFloat("Right", &right, -100.0f, 100.0f);
		ImGui::DragFloat("Top", &top, -100.0f, 100.0f);
		ImGui::DragFloat("Bottom", &bottom, -100.0f, 100.0f);
		ImGui::DragFloat("Near", &orthoNear, -100.0f, 100.0f);
		ImGui::DragFloat("Far", &orthoFar, -100.0f, 100.0f);

		if (ImGui::Button("Change to Perspective", ImVec2(-1, 0))) {
			cameraType = CameraType::Perspective;
		}
	}
}
