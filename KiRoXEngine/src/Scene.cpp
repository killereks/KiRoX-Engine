#include "Scene.h"
#include <imgui.h>
#include "Editor/Console.h"

#include "ext.hpp"
#include "Components/CameraComponent.h"
#include "Components/MeshComponent.h"
#include <unordered_map>
#include "imgui_stdlib.h"
#include "icons/IconsFontAwesome6.h"

#include "Tools/Stopwatch.h"
#include "Macros.h"

Scene::Scene()
{
	//rootEntity = new Entity("Root");
	//entities.push_back(rootEntity);
	//
	//// create some default entities
	//Entity* ent1 = CreateEntity("Entity 1");
	//Entity* ent2 = CreateEntity("Entity 2");
	//Entity* ent3 = CreateEntity("Entity 3");
	//
	//Entity* groundQuad = CreateEntity("Ground");
	//MeshComponent* groundMeshComponent = groundQuad->AddComponent<MeshComponent>();
	//groundQuad->GetTransform().SetLocalRotation(glm::vec3(90, 0, 0));
	//groundQuad->GetTransform().SetLocalScale(glm::vec3(30));
	//
	//Entity* cameraEntity = CreateEntity("Camera");
	//cameraEntity->AddComponent<CameraComponent>();
	//cameraEntity->GetTransform().SetLocalPosition(glm::vec3(0, 5, -10));
	//cameraEntity->GetTransform().LookAt(glm::vec3(0.0));
	//
	//ent2->SetParent(ent1);
}

Scene::~Scene()
{
	delete rootEntity;
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

		if (ImGui::MenuItem("Camera"))
		{
			Entity* ent = CreateEntity("Camera");
			ent->AddComponent<CameraComponent>();
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

	if (rootEntity != nullptr)
	{
		DrawEntity(rootEntity);
	}

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
			ImGui::Text(" " ICON_FA_CARET_RIGHT);
			if (ImGui::IsItemClicked())
			{
				storage->SetBool(id, false);
			}
		}
		else
		{
			ImGui::Text(" " ICON_FA_CARET_DOWN);
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
		ImGui::TextColored(ImVec4(1.0, 1.0, 1.0, 1.0), entity->GetName().c_str(), ImVec2(-1, 0));
	}
	else
	{
		ImGui::TextColored(ImVec4(0.7, 0.7, 0.7, 1.0), entity->GetName().c_str(), ImVec2(-1, 0));
	}
	if (ImGui::IsItemClicked())
	{
		selectedEntity = entity;
	}

	//if (ImGui::BeginDragDropSource())
	//{
	//	const char* payload = entity->GetUUID().str().c_str();
	//	ImGui::SetDragDropPayload("DragEntity", payload, sizeof(payload));
	//	ImGui::EndDragDropSource();
	//}
	//if (ImGui::BeginDragDropTarget())
	//{
	//	const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragEntity");
	//	if (payload != nullptr)
	//	{
	//		const char* uuid = (const char*)payload->Data;
	//
	//		UUIDv4::UUID draggedUUID = UUIDv4::UUID::fromStrFactory(uuid);
	//
	//		Entity* ent = GetEntityByUUID(draggedUUID);
	//		if (ent != nullptr)
	//		{
	//			ent->SetParent(entity);
	//		}
	//	}
	//	ImGui::EndDragDropTarget();
	//}

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
		ImGui::TextColored(ImVec4(0.5, 0.5, 0.5, 1.0), component->GetIcon().c_str(), ImVec2(-1, 0));
		isFirst = false;
	}

	if (!isCollapsed)
	{
		for (auto entity : entity->GetChildren())
		{
			ImGui::PushID(entity->GetName().c_str());
			ImGui::Indent();
			ImGui::Indent();
			DrawEntity(entity);
			ImGui::Unindent();
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

	std::string name = selectedEntity->GetName();
	ImGui::PushItemWidth(-1);
	ImGui::InputText("##Name", &name);
	ImGui::PopItemWidth();
	selectedEntity->SetName(name);

	ImGui::Button(selectedEntity->GetUUID().str().c_str(), ImVec2(-1, 0));

	ImGui::Separator();

	std::string displayName = selectedEntity->GetTransform().GetIcon() + " " + selectedEntity->GetTransform().GetName();

	if (ImGui::CollapsingHeader(displayName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		selectedEntity->GetTransform().DrawInspector();
	}

	int index = 0;
	for (Component* component : selectedEntity->GetAllComponents()) {
		ImGui::PushID(++index);
		std::string displayName = component->GetIcon() + " " + component->GetName();
		if (ImGui::CollapsingHeader(displayName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
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
	if (ImGui::Button(ICON_FA_PLUS " Add Component", ImVec2(-1, 0))) {
		ImGui::OpenPopup("AddComponentPopup");
	}
	ImGui::PopStyleColor();

	if (ImGui::BeginPopup("AddComponentPopup", ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
		ImGui::SeparatorText("General");

		//ImGui::InputText("##componentName", componentName, IM_ARRAYSIZE(componentName));

		if (ImGui::MenuItem("Camera")) {
			selectedEntity->AddComponent<CameraComponent>();
		}

		if (ImGui::MenuItem("Mesh Renderer")) {
			selectedEntity->AddComponent<MeshComponent>();
		}

		ImGui::MenuItem("Light");
		ImGui::Separator();
		ImGui::MenuItem("Rigidbody");
		ImGui::MenuItem("Box Collider");
		ImGui::MenuItem("Sphere Collider");
		ImGui::MenuItem("Capsule Collider");
		ImGui::MenuItem("Mesh Collider");
		ImGui::Separator();
		ImGui::MenuItem("Audio Source");
		ImGui::MenuItem("Audio Listener");
		ImGui::Separator();
		ImGui::MenuItem("Particle System");
		ImGui::Separator();
		ImGui::MenuItem("Animation");
		ImGui::MenuItem("Animator");


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

void Scene::SerializeEntity(YAML::Emitter& out, Entity* ent)
{
	out << YAML::BeginMap;

	out << YAML::Key << "Entity" << YAML::Value << ent->GetName();
	out << YAML::Key << "UUID" << YAML::Value << ent->GetUUID().str();
	if (ent->GetParent() != nullptr)
	{
		out << YAML::Key << "Parent" << YAML::Value << ent->GetParent()->GetUUID().str();
	}

	out << YAML::Key << "Transform";
	out << YAML::BeginMap;
	ent->GetTransform().Serialize(out);
	out << YAML::EndMap;

	for (Component* comp : ent->GetAllComponents())
	{
		out << YAML::Key << comp->GetName();
		out << YAML::BeginMap;
		comp->Serialize(out);
		out << YAML::EndMap;
	}

	out << YAML::EndMap;
}

void Scene::SaveScene(std::string path)
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Scene" << YAML::Value << "Test Scene";

	out << YAML::Key << "Entities" << YAML::Value;
	out << YAML::BeginSeq;

	for (Entity* ent : entities)
	{
		SerializeEntity(out, ent);
	}

	out << YAML::EndSeq;
	out << YAML::EndMap;

	std::ofstream fout(path);
	fout << out.c_str();

	std::cout << "Saved to " << path << std::endl;
}

void Scene::LoadScene(std::string path)
{
	PROFILE_FUNCTION();

	std::ifstream stream(path);
	std::stringstream strStream;
	strStream << stream.rdbuf();

	YAML::Node data = YAML::Load(strStream.str());

	if (!data["Entities"])
	{
		Console::Write("Invalid scene file!");
		return;
	}

	delete rootEntity;
	entities.clear();

	selectedEntity = nullptr;
	rootEntity = nullptr;

	std::string sceneName = data["Scene"].as<std::string>();

	YAML::Node entitiesData = data["Entities"];

	std::unordered_map<std::string, Entity*> uuidToEntity;

	for (auto entityData : entitiesData)
	{
		//std::cout << "Loading new entity\n";

		std::string name = entityData["Entity"].as<std::string>();
		std::string uuid = entityData["UUID"].as<std::string>();

		bool isFirstEntity = entityData == entitiesData[0];
		Entity* newEntity = CreateEntity(name);

		// SET UUID
		UUIDv4::UUID newUUID = UUIDv4::UUID::fromStrFactory(uuid);
		newEntity->SetUUID(newUUID);

		uuidToEntity.emplace(uuid, newEntity);

		if (isFirstEntity)
		{
			rootEntity = newEntity;
			continue;
		}

		YAML::Node parent = entityData["Parent"];

		if (parent)
		{
			Entity* parentEntity = uuidToEntity[parent.as<std::string>()];
			newEntity->SetParent(parentEntity);
		}

		YAML::Node transform = entityData["Transform"];
		if (transform)
		{
			newEntity->GetTransform().SetLocalPosition(transform["position"].as<glm::vec3>());
			newEntity->GetTransform().SetLocalRotation(transform["rotation"].as<glm::quat>());
			newEntity->GetTransform().SetLocalScale(transform["scale"].as<glm::vec3>());
		}

		YAML::Node camera = entityData["Camera"];
		if (camera)
		{
			CameraComponent* cam = newEntity->AddComponent<CameraComponent>();
			// TODO: load
		}

		YAML::Node mesh = entityData["Mesh"];
		if (mesh)
		{
			MeshComponent* meshComp = newEntity->AddComponent<MeshComponent>();

			if (mesh["meshName"])
			{
				meshComp->SetMeshFilter(mesh["meshName"].as<std::string>());
			}
		}
	}
}
