#pragma once

#include "Assets/AssetManager.h"
#include "Editor/Console.h"

#include "Scene.h"
#include "Components/CameraComponent.h"
#include "Tools/FolderWatcher.h"

#include "gizmos/ImGuizmo.h"

enum class SceneState
{
	Editor, Playing, Paused, OneTick
};

class Engine
{
	ImGuizmo::OPERATION currentOperation;
	ImGuizmo::MODE currentMode = ImGuizmo::MODE::WORLD;

	SceneState currentSceneState = SceneState::Editor;

	std::string projectPath;

public:
	Engine();
	~Engine();

	inline static float deltaTime = 0.0f;

	std::shared_ptr<AssetManager> assetManager;
	std::shared_ptr<Scene> scene;

	std::shared_ptr<Scene> activeScene;

	Entity* sceneCamera;
	float pitch, yaw;

	Shader* shader;
	Shader* gizmosShader;

	CameraComponent* sceneCamCache = nullptr;
	inline CameraComponent* GetSceneCamera ()
	{
		if (sceneCamCache == nullptr)
		{
			sceneCamCache = sceneCamera->GetComponent<CameraComponent>();
		}

		return sceneCamCache;
	}

	void SetSceneState(SceneState newSceneState)
	{

		switch (newSceneState)
		{
		case SceneState::Editor:
			// Load Editor Scene
			activeScene = std::make_shared<Scene>();
			activeScene->LoadScene(scene.get()->filePath);
			break;
		case SceneState::Playing:
			// Copy current scene and play it
			std::cout << "Current active scene: " << activeScene.get()->filePath << "\n";
			std::cout << "Playing scene from: " << scene.get()->filePath << "\n";
			activeScene = std::make_shared<Scene>();
			activeScene->LoadScene(scene.get()->filePath);

			ImGui::SetWindowFocus("Game");
			break;
		case SceneState::Paused:
			// Set delta time to 0
			break;
		case SceneState::OneTick:
			// go to next frame
			break;
		default:
			break;
		}
		currentSceneState = newSceneState;
	}

	void Start();
	void Update();

	void RenderScene(Shader* shader);

	void RenderEditorUI();

	void LoadScene(const std::string& path);

	void SceneControls();

	void RenderSceneWindow();
	void RenderGameWindow();

	void RenderToolbar();

	void RenderStatistics();

	void EditTransform(Entity* ent);
};