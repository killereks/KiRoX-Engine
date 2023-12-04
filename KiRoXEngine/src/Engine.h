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
		std::filesystem::path path = std::filesystem::current_path();
		path /= "Project";
		switch (newSceneState)
		{
		case SceneState::Editor:
			// Load Editor Scene
			activeScene = std::make_shared<Scene>();
			activeScene->LoadScene(path.string() + "/test.txt");
			break;
		case SceneState::Playing:
			// Copy current scene and play it
			activeScene = std::make_shared<Scene>();
			activeScene->LoadScene(path.string() + "/test.txt");
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

	void SceneControls();

	void RenderSceneWindow();
	void RenderGameWindow();

	void RenderToolbar();

	void EditTransform(Entity* ent);
};