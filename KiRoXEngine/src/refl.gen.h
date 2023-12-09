#pragma once

#include <rttr/registration>

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

;}

