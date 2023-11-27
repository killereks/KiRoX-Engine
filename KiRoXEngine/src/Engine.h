#pragma once

#include "Assets/AssetManager.h"
#include "Editor/Console.h"

#include "Scene.h"
#include "Components/CameraComponent.h"
#include "Tools/FolderWatcher.h"

#include "gizmos/ImGuizmo.h"

class Engine
{
	ImGuizmo::OPERATION currentOperation;

public:
	Engine();
	~Engine();

	inline static float deltaTime = 0.0f;

	std::shared_ptr<AssetManager> assetManager;
	std::shared_ptr<Scene> scene;

	Entity* sceneCamera;
	float pitch, yaw;

	Shader* shader;

	void Start();
	void Update();

	void RenderScene(CameraComponent* cameraComponent, Shader* shader);

	void RenderEditorUI();

	void SceneControls();

	void RenderSceneWindow();
	void RenderGameWindow();

	void RenderToolbar();

	void EditTransform(Entity* ent);
};