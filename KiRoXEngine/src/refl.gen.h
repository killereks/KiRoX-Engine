#pragma once

#include <rttr/registration>

#include "Components/CameraComponent.h"
#include "Components/Component.h"
#include "Components/MeshComponent.h"
#include "Components/TransformComponent.h"
using namespace rttr;

RTTR_REGISTRATION { 
registration::class_<CameraComponent>("CameraComponent")
.constructor<>()
.property("nearClipPlane", &CameraComponent::nearClipPlane)
.property("farClipPlane", &CameraComponent::farClipPlane)
.property("fieldOfView", &CameraComponent::fieldOfView)
.property("left", &CameraComponent::left)
.property("right", &CameraComponent::right)
.property("top", &CameraComponent::top)
.property("bottom", &CameraComponent::bottom)
.property("orthoNear", &CameraComponent::orthoNear)
.property("orthoFar", &CameraComponent::orthoFar)
;

registration::class_<Component>("Component")
;

registration::class_<MeshComponent>("MeshComponent")
.constructor<>()
.property("meshUUID", &MeshComponent::meshUUID)
.method("UpdateUUID", &MeshComponent::UpdateUUID)
;

registration::class_<TransformComponent>("TransformComponent")
.constructor<>()
.property("position", &TransformComponent::position)
.property("rotation", &TransformComponent::rotation)
.property("scale", &TransformComponent::scale)
;

}

namespace Reflection {

inline CameraComponent* converter_CameraComponent(Component* comp, bool& ok){
ok = true;
return dynamic_cast<CameraComponent*>(comp);
}

inline Component* converter_Component(Component* comp, bool& ok){
ok = true;
return dynamic_cast<Component*>(comp);
}

inline MeshComponent* converter_MeshComponent(Component* comp, bool& ok){
ok = true;
return dynamic_cast<MeshComponent*>(comp);
}

inline TransformComponent* converter_TransformComponent(Component* comp, bool& ok){
ok = true;
return dynamic_cast<TransformComponent*>(comp);
}

static const rttr::type GetType(const std::string& name){
if (name == "CameraComponent"){
return rttr::type::get<CameraComponent*>();
}
if (name == "Component"){
return rttr::type::get<Component*>();
}
if (name == "MeshComponent"){
return rttr::type::get<MeshComponent*>();
}
if (name == "TransformComponent"){
return rttr::type::get<TransformComponent*>();
}
throw std::runtime_error("No type found for " + name);
}

static Component* CreateComponent(const std::string& name){
if (name == "CameraComponent"){
return new CameraComponent();
}
if (name == "MeshComponent"){
return new MeshComponent();
}
if (name == "TransformComponent"){
return new TransformComponent();
}
throw std::runtime_error("No type found for " + name);
}

static void RegisterTypes(){
rttr::type::get<Component*>().register_converter_func(converter_CameraComponent);
rttr::type::get<Component*>().register_converter_func(converter_Component);
rttr::type::get<Component*>().register_converter_func(converter_MeshComponent);
rttr::type::get<Component*>().register_converter_func(converter_TransformComponent);
}

}

