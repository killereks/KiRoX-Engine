#pragma once
#include "Engine.h"
#include "Components/MeshComponent.h"
#include "Tools/Stopwatch.h"
#include <GLFW/glfw3.h>

#include "Tools/Input.h"

#include "Macros.h"
#include "Editor/Gizmos.h"

#include "gizmos/ImGuizmo.h"
#include "glm/gtc/type_ptr.hpp"

Engine::Engine()
{
}

Engine::~Engine()
{
	delete sceneCamera;
	delete shader;
}

void Engine::Start()
{
	std::filesystem::path path = std::filesystem::current_path();
	path /= "Project";

	assetManager = std::make_shared<AssetManager>(path);

	scene = std::make_shared<Scene>();

	sceneCamera = new Entity("Scene Camera");
	sceneCamera->AddComponent<CameraComponent>();

	sceneCamera->GetTransform().SetLocalPosition(glm::vec3(0.0f, 5.0f, -5.0f));
	yaw = 180.0f;
	pitch = -45.0f;

	shader = assetManager->Get<Shader>("TestShader.shader");

	Gizmos::GetInstance()->Init(shader);
}

void Engine::Update()
{
	//PROFILE_FUNCTION()

	assetManager->Update();

	SceneControls();
	RenderEditorUI();
	RenderScene(sceneCamera->GetComponent<CameraComponent>(), shader);

	Gizmos::GetInstance()->Draw();
}

void Engine::RenderScene(CameraComponent* cameraComponent, Shader* shader)
{
	std::vector<MeshComponent*> meshComponents;

	for (auto& entity : scene.get()->GetEntities()) {
		if (entity->HasComponent<MeshComponent>()) {
			meshComponents.push_back(entity->GetComponent<MeshComponent>());
		}
	}

	for (MeshComponent* meshComp : meshComponents) {
		if (meshComp == nullptr) {
			std::cout << "Mesh component is null" << std::endl;
		}
		meshComp->Draw(cameraComponent, shader);
	}
}

void Engine::RenderEditorUI()
{
	assetManager.get()->DrawInspector();
	Console::GetInstance()->Draw();

	scene.get()->DrawHierarchy();
	scene.get()->DrawInspector();

	RenderSceneWindow();
	RenderGameWindow();

	RenderToolbar();

	if (scene->GetSelectedEntity() != nullptr)
	{
		EditTransform(scene->GetSelectedEntity());
	}
}

void Engine::SceneControls()
{
	float mouseSens = -0.1f;

	TransformComponent& transform = sceneCamera->GetTransform();

	if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT)) {
		Input::SetMouseVisibility(false);
	}

	if (Input::GetMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) {
		glm::vec2 mouseDelta = Input::GetMouseDelta();

		pitch += mouseDelta.y * mouseSens;
		yaw += mouseDelta.x * mouseSens;

		pitch = glm::clamp(pitch, -89.0f, 89.0f);
	}

	if (Input::GetMouseButtonUp(GLFW_MOUSE_BUTTON_RIGHT)) {
		Input::SetMouseVisibility(true);
	}

	transform.SetLocalRotation(glm::vec3(pitch, yaw, 0.0f));

	if (Input::GetMouseButton(1))
	{
		float movementSpeed = -3.0f * Engine::deltaTime;

		if (Input::GetKey(GLFW_KEY_LEFT_SHIFT))
		{
			movementSpeed *= 5.0f;
		}

		if (Input::GetKey(GLFW_KEY_W))
		{
			transform.Translate(transform.GetForward() * movementSpeed);
		}
		else if (Input::GetKey(GLFW_KEY_S))
		{
			transform.Translate(transform.GetForward() * -movementSpeed);
		}

		if (Input::GetKey(GLFW_KEY_A))
		{
			transform.Translate(transform.GetRight() * movementSpeed);
		}
		else if (Input::GetKey(GLFW_KEY_D))
		{
			transform.Translate(transform.GetRight() * -movementSpeed);
		}

		if (Input::GetKey(GLFW_KEY_Q))
		{
			transform.Translate(transform.GetUp() * movementSpeed);
		}
		else if (Input::GetKey(GLFW_KEY_E))
		{
			transform.Translate(transform.GetUp() * -movementSpeed);
		}
	}

	ImGui::Begin("Scene Camera Debug");

	glm::vec3 rotation = glm::eulerAngles(transform.GetLocalRotation()) * (180.0f / glm::pi<float>());

	ImGui::Text("Position: %f, %f, %f", transform.GetLocalPosition().x, transform.GetLocalPosition().y, transform.GetLocalPosition().z);
	ImGui::Text("Rotation: %f, %f, %f", rotation.x, rotation.y, rotation.z);
	ImGui::Text("Scale: %f, %f, %f", transform.GetLocalScale().x, transform.GetLocalScale().y, transform.GetLocalScale().z);
	ImGui::Separator();
	ImGui::Text("Forward: %f, %f, %f", transform.GetForward().x, transform.GetForward().y, transform.GetForward().z);
	ImGui::Text("Right: %f, %f, %f", transform.GetRight().x, transform.GetRight().y, transform.GetRight().z);
	ImGui::Text("Up: %f, %f, %f", transform.GetUp().x, transform.GetUp().y, transform.GetUp().z);

	ImGui::End();

	if (!Input::GetMouseButton(1))
	{
		if (Input::GetKeyDown(GLFW_KEY_W))
		{
			currentOperation = ImGuizmo::OPERATION::TRANSLATE;
		}
		if (Input::GetKeyDown(GLFW_KEY_E))
		{
			currentOperation = ImGuizmo::OPERATION::ROTATE;
		}
		if (Input::GetKeyDown(GLFW_KEY_R))
		{
			currentOperation = ImGuizmo::OPERATION::SCALE;
		}
	}
}

void Engine::RenderSceneWindow()
{
	ImGui::Begin("Scene");

	ImGui::End();
}

void Engine::RenderGameWindow()
{
	ImGui::Begin("Game");

	ImGui::End();
}

void Engine::RenderToolbar()
{
	ImGui::Begin("##Toolbar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	if (ImGui::Button("Play")) {
		Console::Write("Playing!");
	}

	ImGui::SameLine();

	if (ImGui::Button("Stop")) {
		Console::Write("Stopping!");
	}
	ImGui::SameLine();

	if (ImGui::Button("Position"))
	{
		currentOperation = ImGuizmo::OPERATION::TRANSLATE;
	}
	ImGui::SameLine();

	if (ImGui::Button("Rotation"))
	{
		currentOperation = ImGuizmo::OPERATION::ROTATE;
	}
	ImGui::SameLine();

	if (ImGui::Button("Scale"))
	{
		currentOperation = ImGuizmo::OPERATION::SCALE;
	}

	ImGui::End();
}

void Engine::EditTransform(Entity* ent)
{
	ImGuiIO& io = ImGui::GetIO();

	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	glm::mat4 viewMatrix = sceneCamera->GetTransform().GetViewMatrix();
	glm::mat4 projection = sceneCamera->GetComponent<CameraComponent>()->GetProjectionMatrix();
	glm::mat4 matrix = ent->GetTransform().GetModelMatrix();

	float* matrixPtr = glm::value_ptr(matrix);
	float* viewMatrixPtr = glm::value_ptr(viewMatrix);
	float* projectionMatrixPtr = glm::value_ptr(projection);

	ImGuizmo::Manipulate(viewMatrixPtr, projectionMatrixPtr, currentOperation, ImGuizmo::WORLD, matrixPtr);

	glm::vec3 translation, rotation, scale;
	ImGuizmo::DecomposeMatrixToComponents(matrixPtr, &translation.x, &rotation.x, &scale.x);

	ent->GetTransform().SetLocalPosition(translation);
	ent->GetTransform().SetLocalRotation(rotation);
	ent->GetTransform().SetLocalScale(scale);
}
