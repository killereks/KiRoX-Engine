#pragma once

#include <rttr/registration>

using namespace rttr;

RTTR_REGISTRATION { 
registration::class_<TransformComponent>("TransformComponent")
.constructor<>()
.property("position", &TransformComponent::position)
.property("rotation", &TransformComponent::rotation)
.property("scale", &TransformComponent::scale)
.method("RotateXTest", &TransformComponent::RotateXTest)
.method("RotateYTest", &TransformComponent::RotateYTest)
.method("RotateZTest", &TransformComponent::RotateZTest)
;}

