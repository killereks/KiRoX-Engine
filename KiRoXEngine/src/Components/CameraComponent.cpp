#include "CameraComponent.h"

#include "imgui.h"
#include "../Macros.h"
#include "../Tools/Stopwatch.h"

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

	renderTexture = new RenderTexture(640, 480);
	aspect = (float)640 / (float)480;
}

CameraComponent::~CameraComponent()
{
	delete renderTexture;
}

void CameraComponent::DrawInspector()
{
	if (cameraType == CameraType::Perspective) {
		ImGui::Text("Perspective Camera");
		ImGui::DragFloat("Near Clip Plane", &nearClipPlane, 0.1f, 100.0f);
		ImGui::DragFloat("Far Clip Plane", &farClipPlane, 0.1f, 100.0f);

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

void CameraComponent::Serialize(YAML::Emitter& out)
{
	SERIALIZE_VALUE(nearClipPlane);
	SERIALIZE_VALUE(farClipPlane);
	SERIALIZE_VALUE(fieldOfView);

	SERIALIZE_VALUE(top);
	SERIALIZE_VALUE(bottom);
	SERIALIZE_VALUE(left);
	SERIALIZE_VALUE(right);
	SERIALIZE_VALUE(orthoNear);
	SERIALIZE_VALUE(orthoFar);
}

void CameraComponent::Bind()
{
	if (renderTexture != nullptr)
	{
		renderTexture->Bind();
	}
}

void CameraComponent::Unbind()
{
	if (renderTexture != nullptr)
	{
		renderTexture->Unbind();
	}
}

void CameraComponent::Clear()
{
	if (renderTexture != nullptr)
	{
		renderTexture->Clear();
	}
}

void CameraComponent::CreateRenderTexture(int width, int height)
{
	if (renderTexture != nullptr)
	{
		delete renderTexture;
	}

	renderTexture = new RenderTexture(width, height);
}

void CameraComponent::Render(std::vector<MeshComponent*>& meshes, Shader* shader)
{
	Bind();
	Clear();
	glViewport(0, 0, width, height);

	shader->setMat4("perspectiveMatrix", GetProjectionMatrix());
	shader->setMat4("viewMatrix", GetViewMatrix());

	// render
	for (MeshComponent* meshComp : meshes)
	{
		meshComp->SimpleDraw(shader);
	}

	Unbind();
}

void CameraComponent::Resize(int width, int height)
{
	this->width = width;
	this->height = height;
	aspect = (float)width / (float)height;
	renderTexture->Resize(width, height);
}
