#include "Scene.h"
#include <imgui.h>
#include "Editor/Console.h"

#include "ext.hpp"
#include "Components/CameraComponent.h"
#include "Components/MeshComponent.h"

Scene::Scene()
{
	rootEntity = new Entity("Root");
	entities.push_back(rootEntity);

	// create some default entities
	Entity* ent1 = CreateEntity("Entity 1");
	Entity* ent2 = CreateEntity("Entity 2");
	Entity* ent3 = CreateEntity("Entity 3");

	Entity* quad = CreateEntity("Quad");
	MeshComponent* meshComponent = quad->AddComponent<MeshComponent>();

	Entity* groundQuad = CreateEntity("Ground");
	MeshComponent* groundMeshComponent = groundQuad->AddComponent<MeshComponent>();
	groundQuad->GetTransform()->SetLocalRotation(glm::vec3(90, 0, 0));
	groundQuad->GetTransform()->SetLocalScale(glm::vec3(20));

	for (int i = 0; i < 5; i++) {
		Entity* newEnt = CreateEntity("Entity " + std::to_string(i));

		int random = rand() % 3;

		if (random == 0) {
			newEnt->SetParent(ent1);
		}
		else if (random == 1) {
			newEnt->SetParent(ent2);
		}
		else {
			newEnt->SetParent(ent3);
		}
	}

	ent2->SetParent(ent1);
}

Scene::~Scene()
{
	for (auto entity : entities)
	{
		delete entity;
	}
}

void Scene::DrawHierarchy()
{
	ImGui::Begin("Hierarchy");

	if (ImGui::Button("Create Entity", ImVec2(-1, 0))) {
		ImGui::OpenPopup("CreateEntityPopup");
	}

	if (ImGui::BeginPopup("CreateEntityPopup", ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
		ImGui::SeparatorText("General");

		if (ImGui::MenuItem("Empty")) {
			CreateEntity("Empty");
		}

		if (ImGui::BeginMenu("3D Object")) {
			if (ImGui::MenuItem("Quad")) {
				Entity* ent = CreateEntity("Quad");
				ent->AddComponent<MeshComponent>();
			}
			ImGui::EndMenu();
		}

		ImGui::EndPopup();
	}

	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0.5f));
	// IndentSpacing
	ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 20);

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.f));

	DrawEntity(rootEntity);

	ImGui::PopStyleColor();

	ImGui::PopStyleVar(3);

	ImGui::End();
}

void Scene::DrawEntity(Entity* entity)
{
	bool hasChildren = entity->GetChildren().size() > 0;

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;

	if (!hasChildren) {
		flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	if (ImGui::CollapsingHeader(entity->GetName().c_str(), flags)) {

		if (ImGui::IsItemClicked()) {
			selectedEntity = entity;
		}

		for (auto entity : entity->GetChildren()) {
			ImGui::PushID(entity->GetName().c_str());
			ImGui::Indent();
			DrawEntity(entity);
			ImGui::Unindent();
			ImGui::PopID();
		}
	}
}

void Scene::DrawInspector()
{
	ImGui::Begin("Inspector");

	if (selectedEntity == nullptr) {
		ImGui::Text("Select an entity!");
		ImGui::End();
		return;
	}

	ImGui::Text(selectedEntity->GetName().c_str());

	ImGui::Separator();

	int index = 0;
	for (Component* component : selectedEntity->GetAllComponents()) {
		ImGui::PushID(++index);
		if (ImGui::CollapsingHeader("Component")) {
			component->DrawInspector();
		}
		ImGui::PopID();
		ImGui::Separator();
	}

	ImGui::Separator();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.f));
	if (ImGui::Button("Add Component", ImVec2(-1, 0))) {
		ImGui::OpenPopup("AddComponentPopup");
	}
	ImGui::PopStyleColor();

	if (ImGui::BeginPopup("AddComponentPopup", ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
		ImGui::SeparatorText("General");

		ImGui::InputText("##componentName", componentName, IM_ARRAYSIZE(componentName));

		if (ImGui::MenuItem("Camera")) {
			selectedEntity->AddComponent<CameraComponent>();
		}

		if (ImGui::MenuItem("Mesh Renderer")) {
			selectedEntity->AddComponent<MeshComponent>();
		}
		ImGui::MenuItem("Light");
		ImGui::MenuItem("Rigidbody");
		ImGui::MenuItem("Box Collider");
		ImGui::MenuItem("Sphere Collider");
		ImGui::MenuItem("Capsule Collider");
		ImGui::MenuItem("Mesh Collider");
		ImGui::MenuItem("Character Controller");
		ImGui::MenuItem("Audio Source");
		ImGui::MenuItem("Audio Listener");
		ImGui::MenuItem("Particle System");
		ImGui::MenuItem("Animation");
		ImGui::MenuItem("Animator");
		ImGui::MenuItem("Nav Mesh Agent");
		ImGui::MenuItem("Nav Mesh Obstacle");
		ImGui::MenuItem("Network Transform");
		ImGui::MenuItem("Network Identity");
		ImGui::MenuItem("Network Animator");
		ImGui::MenuItem("Network Rigid Body");

		ImGui::EndPopup();
	}

	ImGui::End();
}

Entity* Scene::CreateEntity(std::string name)
{
	Entity* entity = new Entity(name);
	entities.push_back(entity);

	entity->SetParent(rootEntity);

	Console::Write("Created a new entity: " + name);

	return entity;
}
