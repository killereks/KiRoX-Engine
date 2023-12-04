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

#include "icons/IconsFontAwesome6.h"

Engine::Engine()
{
}

Engine::~Engine()
{
	delete sceneCamera;
}

void Engine::Start()
{
	std::filesystem::path path = std::filesystem::current_path();
	path /= "Project";

	projectPath = path.string();

	assetManager = std::make_shared<AssetManager>(path);

	scene = std::make_shared<Scene>();

	sceneCamera = new Entity("Scene Camera");
	CameraComponent* cameraComp = sceneCamera->AddComponent<CameraComponent>();
	cameraComp->SetNearClipPlane(0.1f);
	//cameraComp->CreateRenderTexture(1920, 1080);

	sceneCamera->GetTransform().SetLocalPosition(glm::vec3(0.0f, 5.0f, -5.0f));
	yaw = 180.0f;
	pitch = -45.0f;

	shader = assetManager->Get<Shader>("TestShader.shader");

	Gizmos::GetInstance()->Init(shader);

	scene->LoadScene(path.string()+"/test.txt");

	activeScene = std::make_shared<Scene>();
	activeScene->LoadScene(path.string() + "/test.txt");
}

void Engine::Update()
{
	//PROFILE_FUNCTION()

	assetManager->Update();

	SceneControls();
	RenderEditorUI();
	RenderScene(shader);

	if (currentSceneState == SceneState::Playing)
	{
		// simulate physics
		for (Entity* ent : activeScene->GetAllEntities())
		{
			if (ent->HasComponent<MeshComponent>())
			{
				std::cout << "Applying physics on " << ent->GetName() << "\n";
				ent->GetTransform().gravityY += -9.81f * deltaTime;

				ent->GetTransform().Translate(glm::vec3(0.0, ent->GetTransform().gravityY, 0.0f) * deltaTime);

				float y = ent->GetTransform().GetWorldPosition().y;
				if (y <= 0.0f)
				{
					ent->GetTransform().gravityY *= -0.9f;
					y = 0.0f;
				}
			}
		}
	}

	//Gizmos::DrawLine(glm::vec3(0, 1, 0), glm::vec3(2, 2, 0), glm::vec3(1.0, 0.0, 0.0));

	//Gizmos::GetInstance()->Draw(sceneCamera->GetComponent<CameraComponent>());
}

void Engine::RenderScene(Shader* shader)
{
	std::vector<MeshComponent*> meshComponents;

	for (auto& entity : activeScene.get()->GetEntities()) {
		if (entity->HasComponent<MeshComponent>()) {
			meshComponents.push_back(entity->GetComponent<MeshComponent>());
		}
	}

	GetSceneCamera()->Render(meshComponents, shader);

	CameraComponent* gameCamera = activeScene->FindComponentOfType<CameraComponent>();
	if (gameCamera != nullptr)
	{
		gameCamera->Render(meshComponents, shader);
	}
}

void Engine::RenderEditorUI()
{
	assetManager.get()->DrawInspector();
	Console::GetInstance()->Draw();

	activeScene.get()->DrawHierarchy();
	activeScene.get()->DrawInspector();

	RenderSceneWindow();
	RenderGameWindow();

	RenderToolbar();
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
	unsigned int texID = GetSceneCamera()->GetRenderTextureID();

	ImGui::Begin("Scene");
	ImVec2 contentAvail = ImGui::GetContentRegionAvail();
	GetSceneCamera()->Resize(contentAvail.x, contentAvail.y);
	ImGui::Image((void*)texID, contentAvail, ImVec2(0, 1), ImVec2(1, 0));

	if (activeScene->GetSelectedEntity() != nullptr)
	{
		EditTransform(activeScene->GetSelectedEntity());
	}

	ImGui::End();
}

void Engine::RenderGameWindow()
{
	CameraComponent* gameCamera = activeScene->FindComponentOfType<CameraComponent>();

	ImGui::Begin("Game");
	ImVec2 size = ImGui::GetContentRegionAvail();
	if (gameCamera == nullptr)
	{
		ImGui::Button("No active camera in the scene!", size);
	}
	else
	{
		if (gameCamera != nullptr)
		{
			gameCamera->Resize(size.x, size.y);
		}

		unsigned int texID = gameCamera->GetRenderTextureID();
		ImGui::Image((void*)texID, size, ImVec2(0, 1), ImVec2(1, 0));
	}
	ImGui::End();
}

void Engine::RenderToolbar()
{
	ImGuiWindowFlags window_flags = 0
		//| ImGuiWindowFlags_NoDocking
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		//| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoScrollbar;
		//| ImGuiWindowFlags_NoSavedSettings;

	ImGui::Begin("##Toolbar", nullptr, window_flags);

	float size = ImGui::GetContentRegionAvail().x * 0.5f;

	ImGui::SetCursorPosX(size);

	if (currentSceneState == SceneState::Editor)
	{
		if (ImGui::Button(" " ICON_FA_PLAY))
		{
			SetSceneState(SceneState::Playing);
		}
	}
	else
	{
		if (currentSceneState == SceneState::Playing)
		{
			if (ImGui::Button(" " ICON_FA_STOP))
			{
				SetSceneState(SceneState::Editor);
			}
		}
	}

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (currentSceneState == SceneState::Editor)
			{
				if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK " Save", "CTRL+S"))
				{
					activeScene->SaveScene(projectPath + "/test.txt");
				}
				if (ImGui::MenuItem(ICON_FA_FILE_IMPORT " Load", "CTRL+O"))
				{
					activeScene->LoadScene(projectPath + "/test.txt");
				}
			}
			else
			{
				ImGui::Text("Cannot save when in play mode...");
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	// GIZMOS
	ImGui::SameLine();
	if (currentMode == ImGuizmo::MODE::WORLD)
	{
		if (ImGui::Button(ICON_FA_EARTH_EUROPE " World"))
		{
			currentMode = ImGuizmo::MODE::LOCAL;
		}
	}
	else
	{
		if (ImGui::Button(ICON_FA_VECTOR_SQUARE " Local"))
		{
			currentMode = ImGuizmo::MODE::WORLD;
		}
	}

	ImGui::End();
}

void Engine::EditTransform(Entity* ent)
{
	bool useSnap = false;
	glm::vec3 snap;

	if (Input::GetKey(GLFW_KEY_LEFT_CONTROL))
	{
		useSnap = true;
		if (currentOperation == ImGuizmo::OPERATION::ROTATE)
		{
			snap = glm::vec3(45);
		}
		else
		{
			snap = glm::vec3(0.5);
		}
	}

	ImVec2 vMin = ImGui::GetWindowContentRegionMin();
	ImVec2 vMax = ImGui::GetWindowContentRegionMax();

	vMin.x += ImGui::GetWindowPos().x;
	vMin.y += ImGui::GetWindowPos().y;
	vMax.x += ImGui::GetWindowPos().x;
	vMax.y += ImGui::GetWindowPos().y;

	glm::vec4 sceneViewportWindowSize = glm::vec4(vMin.x, vMin.y, vMax.x, vMax.y);

	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(sceneViewportWindowSize.x, sceneViewportWindowSize.y, sceneViewportWindowSize.z - sceneViewportWindowSize.x, sceneViewportWindowSize.w - sceneViewportWindowSize.y);

	glm::mat4 viewMatrix = sceneCamera->GetTransform().GetViewMatrix();
	glm::mat4 projection = GetSceneCamera()->GetProjectionMatrix();
	glm::mat4 matrix = ent->GetTransform().GetModelMatrix();

	float* matrixPtr = glm::value_ptr(matrix);
	float* viewMatrixPtr = glm::value_ptr(viewMatrix);
	float* projectionMatrixPtr = glm::value_ptr(projection);

	ImGuizmo::Manipulate(viewMatrixPtr, projectionMatrixPtr, currentOperation, currentMode, matrixPtr, nullptr, &snap[0]);

	if (ImGuizmo::IsUsing())
	{
		glm::vec3 translation, rotation, scale;
		ImGuizmo::DecomposeMatrixToComponents(matrixPtr, &translation.x, &rotation.x, &scale.x);

		ent->GetTransform().SetLocalPosition(translation);
		ent->GetTransform().SetLocalRotation(rotation);
		ent->GetTransform().SetLocalScale(scale);
	}
}
