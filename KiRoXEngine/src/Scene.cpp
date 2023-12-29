#include "Scene.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include "Editor/Console.h"

#include "ext.hpp"
#include "Components/CameraComponent.h"
#include "Components/MeshComponent.h"
#include <unordered_map>
#include "imgui_stdlib.h"
#include "icons/IconsFontAwesome6.h"

#include "Reflection/PropertyDrawer.h"
#include "refl.gen.h"

#include "Tools/Stopwatch.h"
#include "Tools/StringTools.h"
#include "Macros.h"
#include "Assets/AssetManager.h"

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

	rootEntity = new Entity("Root");
	entities.push_back(rootEntity);
}

Scene::~Scene()
{
	delete rootEntity;
}

co::Coro Scene::BeginLoading()
{
	this->loaded = true;
	return {};
}

void Scene::DrawHierarchy()
{
	ImGui::Begin("Hierarchy");

	if (ImGui::IsMouseReleased(1)) {
		if (ImGui::IsWindowHovered()) {
			ImGui::OpenPopup("CreateEntityPopup");
		}
	}

	if (ImGui::BeginPopup("CreateEntityPopup", ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
		if (ImGui::MenuItem("Empty")) {
			CreateEntity("Empty");
		}

		if (ImGui::MenuItem("Camera"))
		{
			Entity* ent = CreateEntity("Camera");
			ent->AddComponent<CameraComponent>();
		}

		if (ImGui::BeginMenu("3D")) {
			if (ImGui::MenuItem("Empty Mesh")) {
				Entity* ent = CreateEntity("Mesh");
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
			ImGui::Text(ICON_FA_CARET_RIGHT);
			if (ImGui::IsItemClicked())
			{
				storage->SetBool(id, false);
			}
		}
		else
		{
			ImGui::Text(ICON_FA_CARET_DOWN);
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
		ImVec2 boxPos = ImGui::GetCursorScreenPos();
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec2 padding = ImVec2(2, 2);

		boxPos.x -= padding.x;
		boxPos.y -= padding.y;

		ImVec2 endPos = ImVec2(ImGui::GetWindowWidth() + padding.x * 2, ImGui::GetTextLineHeight() + padding.y * 2);
		endPos.x += boxPos.x;
		endPos.y += boxPos.y;

		drawList->AddRectFilled(boxPos, endPos, IM_COL32(51, 51, 51, 255));
		//drawList->AddText(ImVec2(boxPos.x + 5, boxPos.y), IM_COL32(255, 255, 255, 255), entity->GetName().c_str());

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

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY")) {
			Entity* draggedEntity = dynamic_cast<Entity*>(*(Entity**)payload->Data);

			if (draggedEntity != nullptr) {
				draggedEntity->SetParent(entity);
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (entity != rootEntity && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
		Entity** entityPointer = &entity;
		ImGui::SetDragDropPayload("ENTITY", entityPointer, sizeof(entityPointer));
		ImGui::Text("Dragging %s", entity->GetName().c_str());
		ImGui::EndDragDropSource();
	}

	bool isFirst = true;
	// DRAW COMPONENTS
	for (auto component : entity->GetAllComponents())
	{
		if (isFirst)
		{
			ImGui::SameLine(200);
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
		for (auto ent : entity->GetChildren())
		{
			ImGui::PushID(ent->GetName().c_str());
			ImGui::Indent(30.0f);
			DrawEntity(ent);
			ImGui::Unindent(30.0f);
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

	ImGui::Text(selectedEntity->GetUUID().str().c_str(), ImVec2(-1, 0));

	ImGui::Separator();

	// draw TransformComponent
	//const rttr::type& type = rttr::type::get<TransformComponent*>();
	//rttr::variant var = &selectedEntity->GetTransform();
	//var.convert(type);
	//
	//for (rttr::property& prop : type.get_properties()) {
	//	PropertyDrawer::DrawProperty(prop, var);
	//}

	ImGuiStorage* storage = ImGui::GetStateStorage();
	constexpr float ellipsisMenuWidth = 20.0f;
	float width = ImGui::GetContentRegionAvail().x - ellipsisMenuWidth;

	int index = 0;

	for (Component* component : selectedEntity->GetAllComponentsWithTransform()) {
		if (component == nullptr) continue;

		ImGui::PushID(index++);

		ImGuiID id = ImGuiID(component->GetName());
		bool isOpen = storage->GetBool(id, true);

		std::string nameWithIcon = component->GetIcon() + " " + StringTools::FormatComponentName(component->GetName());

		if (isOpen) {
			nameWithIcon = ICON_FA_CARET_DOWN + nameWithIcon;
		}
		else {
			nameWithIcon = ICON_FA_CARET_RIGHT + nameWithIcon;
		}

		ImGui::Text(nameWithIcon.c_str());
		if (ImGui::IsItemClicked()) {
			storage->SetBool(id, !isOpen);
		}
		ImGui::SameLine();

		// CONTEXT MENU
		ImGui::SetCursorPosX(width);

		// Ellipsis button
		ImGui::Text(ICON_FA_ELLIPSIS_VERTICAL);

		if (ImGui::IsItemClicked()) {
			ImGui::OpenPopup("ComponentContextMenu");
		}

		if (isOpen) {
			ImGui::Indent();

			component->OnDrawGizmos();
			component->DrawInspector();

			ImGui::Unindent();
		}

		// Context menu
		if (ImGui::BeginPopup("ComponentContextMenu")) {
			if (ImGui::Button("Remove Component")) {
				selectedEntity->RemoveComponent(component);
				ImGui::CloseCurrentPopup();
			}
			else {
				rttr::type type = rttr::type::get_by_name(component->GetName());
				std::vector<rttr::method> methods = type.get_methods();
				rttr::variant var = component;
				for (auto& method : methods) {
					PropertyDrawer::DrawFunction(method, var);
				}
			}

			ImGui::EndPopup();
		}

		ImGui::PopID();

		ImGui::Separator();
	}

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.f));
	if (ImGui::Button(ICON_FA_PLUS " Add Component")) {
		ImGui::OpenPopup("AddComponentPopup");
	}
	ImGui::PopStyleColor();

	if (ImGui::BeginPopup("AddComponentPopup", ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
		ImGui::SeparatorText("General");

		auto components = rttr::type::get<Component>().get_derived_classes();

		static char searchBuffer[128] = "";
		ImGui::InputText("##ComponentSearch", searchBuffer, sizeof(searchBuffer));

		for (auto& comp : components) {
			if (comp.get_name() == "TransformComponent") continue;

			std::string compName = comp.get_name();
			std::string lowercaseName = StringTools::ToLowerCase(compName);

			if (lowercaseName.find(searchBuffer) != std::string::npos) {
				std::string compDisplayName = StringTools::FormatComponentName(compName.c_str());
				if (ImGui::MenuItem(compDisplayName.c_str())) {
					Component* newCompInstance = Reflection::CreateComponent(comp.get_name());
					selectedEntity->AddComponent(newCompInstance);
				}
			}
		}

		ImGui::EndPopup();
	}

	ImGui::End();
}

Entity* Scene::CreateEntity(std::string name)
{
	Entity* entity = new Entity(name);
	entities.push_back(entity);

	assert(entity != nullptr);
	assert(rootEntity != nullptr);

	entity->SetParent(rootEntity);

	return entity;
}

Entity* Scene::GetEntityByUUID(const UUIDv4::UUID& uuid)
{
	for (Entity* ent : GetAllEntities()) {
		if (ent->GetUUID() == uuid) {
			return ent;
		}
	}

	return nullptr;
}

void Scene::DeleteEntity(Entity* ent)
{
	if (ent == rootEntity) return;

	entities.erase(std::remove(entities.begin(), entities.end(), ent), entities.end());

	Entity* parent = ent->GetParent();
	if (parent != nullptr)
	{
		parent->RemoveChild(ent);
	}

	if (selectedEntity == ent)
	{
		selectedEntity = nullptr;
	}

	delete ent;
}

void Scene::CopyFrom(Scene* other)
{
	PROFILE_FUNCTION()

	if (other == nullptr) return;
	if (other->rootEntity == nullptr) {
		std::cout << "Cannot copy from empty scene!\n";
		return;
	}

	assert(rootEntity != other->rootEntity);

	other->selectedEntity = nullptr;
	selectedEntity = nullptr;

	delete rootEntity;
	entities.clear();

	filePath = other->filePath;
	fileName = other->fileName;
	uuid = other->uuid;

	for (Entity* otherEntity : other->GetAllEntities()) {
		assert(otherEntity != nullptr);

		Entity* newEntity = new Entity(otherEntity->GetName());//CreateEntity(otherEntity->GetName());
		newEntity->SetUUID(otherEntity->GetUUID());

		assert(newEntity->GetName().length() <= 100);

		bool isRoot = other->rootEntity == otherEntity;
		if (isRoot) {
			rootEntity = newEntity;
		}

		TransformComponent& otherTransform = otherEntity->GetTransform();
		TransformComponent& thisTransform = newEntity->GetTransform();

		glm::vec3 pos = otherTransform.GetLocalPosition();
		glm::quat rot = otherTransform.GetLocalRotation();
		glm::vec3 scale = otherTransform.GetLocalScale();
		thisTransform.SetLocalPosition(pos);
		thisTransform.SetLocalRotation(rot);
		thisTransform.SetLocalScale(scale);

		Entity* otherParent = otherEntity->GetParent();
		if (otherParent != nullptr) {
			Entity* parent = GetEntityByUUID(otherParent->GetUUID());
			if (parent != nullptr) {
				newEntity->SetParent(parent);
			}
		}

		entities.push_back(newEntity);

		for (Component* comp : otherEntity->GetAllComponents()) {
			std::string compName = comp->GetName();

			const rttr::type compType = Reflection::GetType(compName);

			Component* newComponentInstance = Reflection::CreateComponent(compName);
			rttr::variant thisVar = newComponentInstance;
			thisVar.convert(compType);

			Component* otherComponentInstance = comp;
			rttr::variant otherVar = otherComponentInstance;
			otherVar.convert(compType);

			for (rttr::property prop : rttr::type::get_by_name(newComponentInstance->GetName()).get_properties()) {
				std::string name = prop.get_name();
				if (name == "") continue;

				rttr::variant value = prop.get_value(otherVar);
				bool setValueSuccess = prop.set_value(thisVar, value);
				assert(setValueSuccess);
			}

			newEntity->AddComponent(newComponentInstance);
		}
	}
}

void Scene::DuplicateEntity(Entity* entity)
{
	std::string newName = entity->GetName() + " (Copy)";
	Entity* ent = CreateEntity(newName);
	ent->CopyFrom(entity);
	selectedEntity = ent;
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

	out << YAML::Key << "TransformComponent";
	out << YAML::BeginMap;

	TransformComponent& transform = ent->GetTransform();
	out << YAML::Key << "position" << YAML::Value << transform.GetLocalPosition();
	out << YAML::Key << "rotation" << YAML::Value << transform.GetLocalRotation();
	out << YAML::Key << "scale" << YAML::Value << transform.GetLocalScale();

	out << YAML::EndMap;

	for (Component* comp : ent->GetAllComponents()) {
		rttr::type t = rttr::type::get_by_name(comp->GetName());

		out << YAML::Key << comp->GetName();
		out << YAML::BeginMap;
		for (rttr::property prop : t.get_properties()) {
			std::string name = prop.get_name();
			if (name == "") continue;
			out << YAML::Key << name;
			out << YAML::Value << prop.get_value(comp).to_string();
		}
		out << YAML::EndMap;
	}

	out << YAML::EndMap;
}

void Scene::SaveScene(std::string path)
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Entities" << YAML::Value;
	out << YAML::BeginSeq;

	//SerializeEntity(out, rootEntity);

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

void Scene::CubeScene()
{
	// make a default scene, which is 20x20x20 default cube
	int size = 10;

	for (int i = -size / 2; i <= size / 2; i++) {
		for (int j = -size / 2; j <= size / 2; j++) {
			for (int k = -size / 2; k <= size / 2; k++) {
				glm::vec3 pos = glm::vec3(i, j, k) * 5.0f;

				std::string formattedName = "Cube " + std::to_string(i) + "_" + std::to_string(j) + "_" + std::to_string(k);

				Entity* ent = CreateEntity(formattedName);
				ent->GetTransform().SetLocalPosition(pos);

				ent->AddComponent<Rigidbody>();
				ent->AddComponent<BoxCollider>();

				MeshComponent* meshComponent = ent->AddComponent<MeshComponent>();
				meshComponent->SetMeshUUID("6f96cb9a-ad5a-4a5b-bc26-c7ebf27fb931");
			}
		}
	}
}

void Scene::LoadScene(std::string path)
{
	PROFILE_FUNCTION();

	std::cout << "Loading scene from path: " << path << "\n";

	filePath = path;
	size_t lastSlash = path.rfind("\\");
	if (lastSlash == std::string::npos) lastSlash = 0;
	fileName = path.substr(lastSlash + 1);

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

	YAML::Node entitiesData = data["Entities"];

	std::unordered_map<std::string, Entity*> uuidToEntity;

	for (auto entityData : entitiesData)
	{
		std::string name = entityData["Entity"].as<std::string>();
		std::string uuid = entityData["UUID"].as<std::string>();

		bool isFirstEntity = entityData == entitiesData[0];
		Entity* newEntity = new Entity(name); //CreateEntity(name);

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

		// iterate over components
		auto compList = rttr::type::get<Component>().get_derived_classes();

		for (auto& comp : compList) {
			std::string strCompName = comp.get_name();

			if (strCompName == "TransformComponent") continue;

			// Component name
			YAML::Node componentRef = entityData[strCompName];
			if (!componentRef) continue;

			// using rttr, create a new component
			Component* componentInstance = Reflection::CreateComponent(strCompName);
			rttr::variant var = componentInstance;

			// iterate over properties
			for (rttr::property currentProperty : comp.get_properties()) {
				std::string name = currentProperty.get_name();
				if (name == "") continue;

				// variable inside the component
				YAML::Node propData = componentRef[name];
				if (!propData) continue;

				rttr::type propType = currentProperty.get_type();

				if (propType == rttr::type::get<std::string>()) {
					currentProperty.set_value(var, propData.as<std::string>());
				}
				else if (propType == rttr::type::get<int>()) {
					currentProperty.set_value(var, propData.as<int>());
				} 
				else if (propType == rttr::type::get<float>()) {
					currentProperty.set_value(var, propData.as<float>());
				}
				else if (propType == rttr::type::get<bool>()) {
					currentProperty.set_value(var, propData.as<bool>());
				}
				else if (propType == rttr::type::get<glm::vec3>()) {
					currentProperty.set_value(var, propData.as<glm::vec3>());
				}
				else if (propType == rttr::type::get<glm::vec4>()) {
					currentProperty.set_value(var, propData.as<glm::vec4>());
				}
				else if (propType == rttr::type::get<glm::quat>()) {
					currentProperty.set_value(var, propData.as<glm::quat>());
				}
			}

			if (componentInstance) {
				newEntity->AddComponent(componentInstance);
			}
			else {
				std::cout << "Error, could not add " << strCompName << " component to entity " << name << "\n";
			}
		}

		// Transform is different, so we load it separately
		YAML::Node transform = entityData["TransformComponent"];
		if (transform)
		{
			newEntity->GetTransform().SetLocalPosition(transform["position"].as<glm::vec3>());
			newEntity->GetTransform().SetLocalRotation(transform["rotation"].as<glm::quat>());
			newEntity->GetTransform().SetLocalScale(transform["scale"].as<glm::vec3>());
		}
	}
}
