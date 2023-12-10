#pragma once

#include <rttr/registration>

class MeshComponent;
class TransformComponent;
using namespace rttr;

RTTR_REGISTRATION { 
registration::class_<MeshComponent>("MeshComponent")
.constructor<>()
.property("myNumber", &MeshComponent::myNumber)
;

registration::class_<TransformComponent>("TransformComponent")
.constructor<>()
.property("position", &TransformComponent::position)
.property("rotation", &TransformComponent::rotation)
.property("scale", &TransformComponent::scale)
;

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
if (name == "MeshComponent"){
return rttr::type::get<MeshComponent*>();
}
if (name == "TransformComponent"){
return rttr::type::get<TransformComponent*>();
}
throw std::runtime_error("No type found for " + name);
}

static void RegisterTypes(){
rttr::type::get<Component*>().register_converter_func(converter_MeshComponent);
rttr::type::get<Component*>().register_converter_func(converter_TransformComponent);
}

