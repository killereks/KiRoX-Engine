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

	Entity* groundQuad = CreateEntity("Ground");
	MeshComponent* groundMeshComponent = groundQuad->AddComponent<MeshComponent>();
	groundQuad->GetTransform().SetLocalRotation(glm::vec3(90, 0, 0));
	groundQuad->GetTransform().SetLocalScale(glm::vec3(20));

	ent2->SetParent(ent1);

	SaveScene("test.scene");
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

	// COLLAPSE BUTTON
	ImGuiStorage* storage = ImGui::GetStateStorage();
	ImGuiID id = ImGui::GetID("CollapseButton");
	bool isCollapsed = storage->GetBool(id);

	ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5, 0.5));

	if (hasChildren)
	{
		if (isCollapsed)
		{
			ImGui::Text(">");
			if (ImGui::IsItemClicked())
			{
				storage->SetBool(id, false);
			}
		}
		else
		{
			ImGui::Text("\\/");
			if (ImGui::IsItemClicked())
			{
				storage->SetBool(id, true);
			}
		}
		ImGui::SameLine();
	}

	ImGui::PopStyleVar();

	// DRAW ENTITY
	if (selectedEntity == entity)
	{
		ImGui::TextColored(ImVec4(1.0, 1.0, 1.0, 1.0), ("-> "+entity->GetName()).c_str(), ImVec2(-1, 0));
	}
	else
	{
		ImGui::TextColored(ImVec4(0.7, 0.7, 0.7, 1.0), entity->GetName().c_str(), ImVec2(-1, 0));
	}
	if (ImGui::IsItemClicked())
	{
		selectedEntity = entity;
	}

	bool isFirst = true;
	// DRAW COMPONENTS
	for (auto component : entity->GetAllComponents())
	{
		if (isFirst)
		{
			ImGui::SameLine(300);
		}
		else
		{
			ImGui::SameLine();
		}
		ImGui::TextColored(ImVec4(0.5, 0.5, 0.5, 1.0), component->GetName(), ImVec2(-1, 0));
		isFirst = false;
	}

	if (!isCollapsed)
	{
		for (auto entity : entity->GetChildren())
		{
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
	ImGui::Text(selectedEntity->GetUUID().str().c_str());

	ImGui::Separator();

	if (ImGui::CollapsingHeader(selectedEntity->GetTransform().GetName(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		selectedEntity->GetTransform().DrawInspector();
	}

	int index = 0;
	for (Component* component : selectedEntity->GetAllComponents()) {
		ImGui::PushID(++index);
		if (ImGui::CollapsingHeader(component->GetName(), ImGuiTreeNodeFlags_DefaultOpen)) {
			component->DrawInspector();
		}
		if (ImGui::Button("X"))
		{
			selectedEntity->RemoveComponent(component);
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

	return entity;
}

void Scene::SaveScene(std::string path)
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Scene" << YAML::Value << "Test Scene";
	out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

	// TO DO

	out << YAML::EndSeq;
	out << YAML::EndMap;

	std::ofstream fout(path);
	fout << out.c_str();

	std::cout << "Saved to " << path << std::endl;
}

void Scene::LoadScene(std::string path)
{
}
