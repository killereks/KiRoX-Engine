#pragma once
#include "Engine.h"
#include "Components/MeshComponent.h"
#include "Tools/Stopwatch.h"
#include <GLFW/glfw3.h>

#include "Tools/Input.h"

#include "Macros.h"
#include "Editor/Gizmos.h"

#include "imgui_internal.h"
#include <imgui/notifications/ImGuiNotify.hpp>

#include "gizmos/ImGuizmo.h"
#include "glm/gtc/type_ptr.hpp"

#include "icons/IconsFontAwesome6.h"
#include "Tools/StatsCounter.h"
#include "Math/Mathf.h"

#include "Tools/StringTools.h"

#include <ShObjIdl.h>
#include <Components/Light/DirectionalLight.h>

#include <Rendering/RenderTools.h>

#include <Tools/SavingLoading.h>

Engine::Engine()
{
	RenderTools::Initialize();
}

Engine::~Engine()
{
	delete sceneCamera;
	RenderTools::Destroy();

	delete shadowMapper;
}

void Engine::BeginFrame()
{
#if EDITOR
	Profiler::GetInstance()->BeginFrame();
#endif
}

void Engine::EndFrame()
{
#if EDITOR
	Profiler::GetInstance()->EndFrame();
#endif
}

void Engine::OnScenePlay()
{
	if (physics != nullptr) {
		delete physics;
	}

	physics = new Physics();

	// initialize rigidbodies
	std::vector<Rigidbody*> rbs = activeScene.get()->FindComponentsOfType<Rigidbody>();
	for (Rigidbody* rb : rbs) {
		physics->RegisterRigidbody(rb);
	}

	// init all components
	std::vector<Entity*> allEntities = activeScene.get()->GetAllEntities();
	for (Entity* ent : allEntities) {
		const std::vector<Component*>& components = ent->GetAllComponents();

		for (Component* comp : components) {
			comp->Init();
		}
	}
}

void Engine::Start()
{
	Reflection::RegisterTypes();

	std::filesystem::path path = std::filesystem::current_path();
#if EDITOR
	Console::SetOutputPath(path.string() + "\\editor.log");
#endif

	path /= "Project";

	projectPath = path.string();

	if (!std::filesystem::exists(projectPath)) {
		std::cout << "Project folder not found. Expected at " << projectPath << std::endl;
	}

	assetManager = std::make_shared<AssetManager>(path);

#if EDITOR
	assetManager->AddCallback_OnLoaded("Sphere.fbx", [&]() {
		DrawMaterialPreviews();
		});
#endif

	scene = std::make_shared<Scene>();
	activeScene = std::make_shared<Scene>();

	sceneCamera = new Entity("Scene Camera");
	CameraComponent* cameraComp = sceneCamera->AddComponent<CameraComponent>();
	cameraComp->SetNearClipPlane(0.1f);
	cameraComp->SetFarClipPlane(2500.0f);
	cameraComp->ForceUseSkybox(true);

	sceneCamera->GetTransform().SetLocalPosition(glm::vec3(0.0f, 5.0f, -5.0f));
	yaw = 180.0f;
	pitch = -45.0f;

	shader = assetManager->Get<Shader>("TestShader.shader");
	gizmosShader = assetManager->Get<Shader>("Gizmos.shader");

	Gizmos::GetInstance()->Init(gizmosShader);

	shadowMapper = new SceneShadowMapper();

	const std::string scenePath = projectPath + "\\lightTest.scene";
	LoadScene(scenePath);

#ifndef EDITOR
	SetSceneState(SceneState::Playing);
#endif
}

void Engine::Update()
{
	PROFILE_FUNCTION()
	assetManager->Update();

	shader = assetManager->Get<Shader>("TestShader.shader");

#if EDITOR
	SceneControls();
	//shadowMapper->DrawDebug();
#endif
	RenderScene(shader);
#if EDITOR
	RenderEditorUI();
#endif

	if (currentSceneState == SceneState::Playing)
	{
		// simulate physics
		physics->Update(Engine::deltaTime);

		std::vector<Entity*> allEntities = activeScene.get()->GetAllEntities();

		for (Entity* ent : allEntities) {
			const std::vector<Component*>& components = ent->GetAllComponents();
			
			for (Component* comp : components) {
				comp->Update(Engine::deltaTime);
			}
		}

		#if EDITOR
		if (Input::GetKey(GLFW_KEY_ESCAPE)) {
			Input::SetMouseVisibility(true);
		}
		#endif
	}

#if EDITOR
	Gizmos::DrawLine(glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	Gizmos::DrawLine(glm::vec3(0.0), glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0));
	Gizmos::DrawLine(glm::vec3(0.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 1.0));
#endif
}

void Engine::RenderScene(Shader* shader)
{
	PROFILE_FUNCTION()
	// Meshes
	std::vector<MeshComponent*> meshComponents = activeScene.get()->FindComponentsOfType<MeshComponent>();

	// Lights
	DirectionalLight* dirLight = activeScene.get()->FindComponentOfType<DirectionalLight>();
	std::vector<SpotLight*> spotLights = activeScene.get()->FindComponentsOfType<SpotLight>();

	// Post Processing
	Volume* volume = activeScene.get()->FindComponentOfType<Volume>();

	StatsCounter::GetInstance()->SetCounter("meshEntities", meshComponents.size());

	CameraComponent* gameCamera = activeScene->FindComponentOfType<CameraComponent>();

	// SHADOW MAP PASS
	//shadowMapper->BeginShadowMapping();
	if (dirLight != nullptr && gameCamera != nullptr) {
		dirLight->RenderTightCamera(meshComponents, gameCamera);
	}

	//for (SpotLight* spotLight : spotLights) {
	//	shadowMapper->RenderSpotLight(spotLight, meshComponents);
	//}
	//shadowMapper->EndShadowMapping();
	// END SHADOW MAP PASS

	shader->use();
	shader->setInt("shadowMap", 1);
	if (dirLight != nullptr && gameCamera != nullptr) {
		dirLight->BindShadowMap(1);
		shader->setMat4("lightSpaceMatrix", dirLight->GetTightLightSpaceMatrix(gameCamera));
		shader->setVec3("lightDir", dirLight->GetOwner()->GetTransform().GetForward());
	}
	shader->setVec3("viewPos", sceneCamera->GetTransform().GetWorldPosition());
	shader->setVec3("viewForward", sceneCamera->GetTransform().GetForward());

	glCullFace(GL_BACK);

#if EDITOR
	GetSceneCamera()->PreRender();
	TryRenderSelectedEntity();
	GetSceneCamera()->RenderUsingMaterials(meshComponents);
	GetSceneCamera()->RenderGizmos();
	GetSceneCamera()->PostRender();
	if (volume != nullptr) {
		volume->Apply(GetSceneCamera()->GetRenderTextureID(),
			GetSceneCamera()->GetScreenWidth(),
			GetSceneCamera()->GetScreenHeight(),
			GetSceneCamera(),
			dirLight);
	}
#endif
	if (gameCamera != nullptr)
	{
		#ifndef EDITOR
		glm::vec2 size = Input::GetInstance()->GetWindowSize();
		gameCamera->Resize(size.x, size.y);
		#endif
		shader->use();
		shader->setInt("shadowMap", 1);
		if (dirLight != nullptr) {
			dirLight->BindShadowMap(1);
			shader->setMat4("lightSpaceMatrix", dirLight->GetTightLightSpaceMatrix(gameCamera));
			shader->setVec3("lightDir", dirLight->GetOwner()->GetTransform().GetForward());
		}
		shader->setVec3("viewPos", gameCamera->GetOwner()->GetTransform().GetWorldPosition());
		shader->setVec3("viewForward", gameCamera->GetOwner()->GetTransform().GetForward());

		gameCamera->PreRender();
		// TODO: Resize to screen width and height
		gameCamera->RenderUsingMaterials(meshComponents);
		gameCamera->PostRender();

		if (volume != nullptr) {
			volume->Apply(gameCamera->GetRenderTextureID(),
							gameCamera->GetScreenWidth(),
							gameCamera->GetScreenHeight(),
							gameCamera,
							dirLight);
		}

		#ifndef EDITOR
			Shader* quadShader = assetManager->Get<Shader>("Quad.shader");
			quadShader->use();
			RenderTools::DrawScreenQuad(gameCamera->GetRenderTextureID(), quadShader);
		#endif
	}

	Gizmos::GetInstance()->Clear();
}

void Engine::TryRenderSelectedEntity()
{
	PROFILE_FUNCTION()
	Entity* selectedEntity = activeScene.get()->GetSelectedEntity();

	if (selectedEntity == nullptr) return;

	MeshComponent* meshComp = selectedEntity->GetComponent<MeshComponent>();

	if (meshComp == nullptr) return;

	Shader* outlineShader = assetManager->Get<Shader>("Outline.shader");

	outlineShader->use();

	outlineShader->setMat4("viewMatrix", GetSceneCamera()->GetViewMatrix());
	outlineShader->setMat4("projectionMatrix", GetSceneCamera()->GetProjectionMatrix());

	glLineWidth(5.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glCullFace(GL_FRONT);
	meshComp->SimpleDraw(outlineShader);
	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glLineWidth(1.0f);
}

void Engine::RenderEditorUI()
{
	PROFILE_FUNCTION()

	assetManager.get()->DrawInspector();
	assetManager.get()->DrawImportSettings();

	Console::GetInstance()->Draw();

	activeScene.get()->DrawHierarchy();
	activeScene.get()->DrawInspector();

	RenderSceneWindow();
	RenderGameWindow();

	RenderToolbar();
}

void Engine::LoadScene(const std::string& path)
{
	PROFILE_FUNCTION()
	scene = std::make_shared<Scene>();
	scene.get()->LoadScene(path);
	activeScene = std::make_shared<Scene>();
	activeScene.get()->CopyFrom(scene.get());

	ImGui::InsertNotification({ImGuiToastType::Info, 3000, "Scene %s loaded successfully!", scene->fileName.c_str()});
}

void Engine::SceneControls()
{
	PROFILE_FUNCTION()
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantTextInput) return;

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

	Entity* selectedEntity = activeScene->GetSelectedEntity();
	if (selectedEntity != nullptr)
	{
		if (Input::GetKeyDown(GLFW_KEY_DELETE))
		{
			activeScene->DeleteEntity(selectedEntity);
		}
		if (Input::GetKeyDown(GLFW_KEY_D) && Input::GetKey(GLFW_KEY_LEFT_CONTROL)) {
			activeScene->DuplicateEntity(selectedEntity);
		}
	}
}

void Engine::RenderSceneWindow()
{
	PROFILE_FUNCTION()
	unsigned int texID = GetSceneCamera()->GetRenderTextureID();

	ImGui::Begin("Scene");

	ImVec2 pos = ImGui::GetCursorPos();
	ImGui::Dummy(ImGui::GetContentRegionAvail());
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET")) {
			Scene* scene = dynamic_cast<Scene*>(*(Asset**)payload->Data);

			if (scene != nullptr) {
				LoadScene(scene->filePath);
			}

			Material* mat = dynamic_cast<Material*>(*(Asset**)payload->Data);

			if (mat != nullptr) {
				ImVec2 windowSize = ImGui::GetWindowSize();
				ImVec2 windowPos = ImGui::GetWindowPos();

				mouseSelecting.Resize(windowSize.x, windowSize.y);

				glm::vec2 mousePosRelative = glm::vec2(ImGui::GetMousePos().x - windowPos.x, ImGui::GetMousePos().y - windowPos.y);
				std::vector<MeshComponent*> meshComponents = activeScene.get()->FindComponentsOfType<MeshComponent>();

				glm::vec3 color = mouseSelecting.MouseClicked(meshComponents, GetSceneCamera(), mousePosRelative);

				activeScene.get()->SelectEntityByIndex(color.x);
				Entity* selectedEntity = activeScene.get()->GetSelectedEntity();
				if (selectedEntity != nullptr) {
					if (selectedEntity->HasComponent<MeshComponent>()) {
						selectedEntity->GetComponent<MeshComponent>()->SetMaterial(mat);
					}
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
	ImGui::SetCursorPos(pos);

	ImVec2 contentAvail = ImGui::GetContentRegionAvail();
	GetSceneCamera()->Resize(contentAvail.x, contentAvail.y);
	ImGui::Image((void*)texID, contentAvail, ImVec2(0, 1), ImVec2(1, 0));

	if (activeScene->GetSelectedEntity() != nullptr)
	{
		EditTransform(activeScene->GetSelectedEntity());
	}

	RenderStatistics();

	if (ImGui::IsItemHovered() && !ImGuizmo::IsUsing()) {
		// click to select
		if (Input::GetMouseButtonDown(0)) {
			ImVec2 windowSize = ImGui::GetWindowSize();
			ImVec2 windowPos = ImGui::GetWindowPos();

			mouseSelecting.Resize(windowSize.x, windowSize.y);

			glm::vec2 mousePosRelative = glm::vec2(ImGui::GetMousePos().x - windowPos.x, ImGui::GetMousePos().y - windowPos.y);
			std::vector<MeshComponent*> meshComponents = activeScene.get()->FindComponentsOfType<MeshComponent>();

			glm::vec3 color = mouseSelecting.MouseClicked(meshComponents, GetSceneCamera(), mousePosRelative);

			activeScene.get()->SelectEntityByIndex(color.x);
		}
	}

	ImGui::End();
}

void Engine::RenderGameWindow()
{
	PROFILE_FUNCTION()
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
	PROFILE_FUNCTION()
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
				if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK " Save As...", "CTRL+S"))
				{
					std::string path;

					// Open save file dialog
					HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
					if (SUCCEEDED(hr)) {
						IFileSaveDialog* pFileSave;

						hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

						if (SUCCEEDED(hr)) {
							// Set default extension to .scene
							COMDLG_FILTERSPEC fileTypes[] = { { L"Scene Files", L"*.scene" } };
							pFileSave->SetFileTypes(1, fileTypes);
							pFileSave->SetDefaultExtension(L"scene");

							hr = pFileSave->Show(NULL);

							if (SUCCEEDED(hr)) {
								IShellItem* pItem;
								hr = pFileSave->GetResult(&pItem);
								if (SUCCEEDED(hr)) {
									LPWSTR pszFilePath;
									hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

									if (SUCCEEDED(hr)) {
										std::wstring ws(pszFilePath);
										std::string str(ws.begin(), ws.end());

										path = str;
										CoTaskMemFree(pszFilePath);
									}
									pItem->Release();
								}
							}
							pFileSave->Release();
						}
					}

					if (!path.empty()) {
						activeScene->SaveScene(path);
					}
				}

				if (ImGui::MenuItem(ICON_FA_FILE_IMPORT " Open...", "CTRL+O"))
				{
					//activeScene->LoadScene(projectPath + "/serialization_test.scene");

					// open load file dialog
					std::string path;

					HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

					if (SUCCEEDED(hr))
					{
						IFileOpenDialog* pFileOpen;

						hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

						if (SUCCEEDED(hr))
						{
							hr = pFileOpen->Show(NULL);

							if (SUCCEEDED(hr))
							{
								IShellItem* pItem;
								hr = pFileOpen->GetResult(&pItem);
								if (SUCCEEDED(hr))
								{
									LPWSTR pszFilePath;
									hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

									if (SUCCEEDED(hr))
									{
										std::wstring ws(pszFilePath);
										std::string str(ws.begin(), ws.end());

										path = str;
										CoTaskMemFree(pszFilePath);
									}
									pItem->Release();
								}
							}
							pFileOpen->Release();
						}
					}

					if (path != "")
					{
						activeScene->LoadScene(path);
					}
				}
				if (ImGui::MenuItem(ICON_FA_CUBES_STACKED " Add Stress Test Cube Scene"))
				{
					activeScene->CubeScene();
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
	if (currentGizmoMode == ImGuizmo::MODE::WORLD)
	{
		if (ImGui::Button(ICON_FA_EARTH_EUROPE " World"))
		{
			currentGizmoMode = ImGuizmo::MODE::LOCAL;
		}
	}
	else
	{
		if (ImGui::Button(ICON_FA_VECTOR_SQUARE " Local"))
		{
			currentGizmoMode = ImGuizmo::MODE::WORLD;
		}
	}

	ImGui::SameLine();
	// reload assets button
	if (ImGui::Button(ICON_FA_ROTATE_RIGHT" Reload Assets"))
	{
		DrawMaterialPreviews();
	}

	ImGui::End();
}

void Engine::RenderStatistics()
{
	PROFILE_FUNCTION()
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoNav |
		ImGuiWindowFlags_NoMove;

	const float padding = 15.0f;

	ImVec2 windowPos = ImGui::GetWindowPos();
	windowPos.x += padding;
	windowPos.y += ImGui::GetWindowHeight() - padding;

	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, ImVec2(0,1));

	ImGui::SetNextWindowBgAlpha(0.8f);

	StatsCounter* counter = StatsCounter::GetInstance();

	ImGui::Begin("Statistics", nullptr, window_flags);
	ImGui::TextColored(ImVec4(0.204f, 0.596f, 0.859f, 1.0), "Statistics");
	ImGui::Separator();
	ImGui::Text("Frame time: %.5f ms", Engine::deltaTime * 1000.0f);
	ImGui::Text("Estimated FPS: %.2f", 1.0f / Engine::deltaTime);
	ImGui::Text("Draw Calls: %s", Mathf::FormatWithCommas(counter->GetCounter("drawCalls")).c_str());
	ImGui::Text("Triangles: %s", Mathf::FormatWithCommas(counter->GetCounter("triangles")).c_str());
	ImGui::Text("Triangles culled: %s", Mathf::FormatWithCommas(counter->GetCounter("culledTriangles")).c_str());
	ImGui::Text("Total mesh entities: %s", Mathf::FormatWithCommas(counter->GetCounter("meshEntities")).c_str());
	ImGui::End();

	counter->Reset();
}

void Engine::DrawMaterialPreviews()
{
	PROFILE_FUNCTION()
	std::vector<Material*> materials = assetManager->GetAssetsOfType<Material>();

	Shader* shader = assetManager->Get<Shader>("TestShader.shader");

	shader->use();

	Scene* tempScene = new Scene();

	Entity* sphereEnt = tempScene->CreateEntity("Sphere");
	MeshComponent* meshComp = sphereEnt->AddComponent<MeshComponent>();
	meshComp->SetMesh(assetManager->Get<MeshFilter>("Sphere.fbx"));

	Entity* lightEnt = tempScene->CreateEntity("Light");
	DirectionalLight* lightComp = lightEnt->AddComponent<DirectionalLight>();
	lightComp->GetOwner()->GetTransform().SetWorldPosition(glm::vec3(-10.0f, -10.0f, -10.0f));
	lightComp->GetOwner()->GetTransform().SetWorldRotation(glm::vec3(45.0f, 45.0f, 0.0f));

	Entity* cameraEnt = tempScene->CreateEntity("Camera");
	CameraComponent* cameraComp = cameraEnt->AddComponent<CameraComponent>();
	cameraComp->SetNearClipPlane(0.1f);
	cameraComp->SetFarClipPlane(250.0f);
	cameraComp->GetOwner()->GetTransform().SetWorldPosition(glm::vec3(0.0f, 0.25f, -2.5f));
	cameraComp->GetOwner()->GetTransform().LookAt(glm::vec3(0.0f, 0.0f, 0.0f));

	Entity* quadEnt = tempScene->CreateEntity("Quad");
	MeshComponent* quadMeshComp = quadEnt->AddComponent<MeshComponent>();
	quadMeshComp->SetMesh(assetManager->Get<MeshFilter>("quad.fbx"));
	quadMeshComp->GetOwner()->GetTransform().SetWorldPosition(glm::vec3(0.0f, -1.25f, 0.0f));
	quadMeshComp->GetOwner()->GetTransform().SetWorldScale(glm::vec3(10.0f, 10.0f, 10.0f));
	quadMeshComp->GetOwner()->GetTransform().SetWorldRotation(glm::vec3(90.0f, 0.0f, 0.0f));

	int previewWidth = 512;
	int previewHeight = 512;

	for (Material* mat : materials) {
		lightComp->Render(meshComp);

		shader->use();
		shader->setVec3("lightDir", lightComp->GetOwner()->GetTransform().GetForward());
		shader->setVec3("viewPos", cameraComp->GetOwner()->GetTransform().GetWorldPosition());
		shader->setVec3("viewForward", cameraComp->GetOwner()->GetTransform().GetForward());

		shader->setInt("shadowMap", 1);
		lightComp->BindShadowMap(1);
		shader->setMat4("lightSpaceMatrix", lightComp->GetLightSpaceMatrix());

		meshComp->SetMaterial(mat);

		cameraComp->Resize(previewWidth, previewHeight);
		cameraComp->PreRender();
		// #282828 in normalized rgba
		cameraComp->ClearColor(glm::vec4(0.156f, 0.156f, 0.156f, 1.0f));
		cameraComp->ForceRenderWithMaterial(meshComp, mat);
		cameraComp->ForceRenderWithMaterial(quadMeshComp, mat);
		cameraComp->PostRender();

		unsigned int cameraTexID = cameraComp->GetRenderTextureID();

		mat->CopyPreviewTextureFromID(cameraTexID, previewWidth, previewHeight);
	}

	tempScene->SaveScene(projectPath + "\\previewScene.scene");

	delete tempScene;
}

void Engine::EditTransform(Entity* ent)
{
	PROFILE_FUNCTION()
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

	if (useSnap) {
		ImGuizmo::Manipulate(viewMatrixPtr, projectionMatrixPtr, currentOperation, currentGizmoMode, matrixPtr, nullptr, &snap[0]);
	}
	else {
		ImGuizmo::Manipulate(viewMatrixPtr, projectionMatrixPtr, currentOperation, currentGizmoMode, matrixPtr, nullptr, nullptr);
	}

	if (ImGuizmo::IsUsing())
	{
		glm::vec3 translation, rotation, scale;
		ImGuizmo::DecomposeMatrixToComponents(matrixPtr, &translation.x, &rotation.x, &scale.x);

		ent->GetTransform().SetWorldPosition(translation);
		ent->GetTransform().SetWorldRotation(rotation);
		ent->GetTransform().SetWorldScale(scale);
	}
}
