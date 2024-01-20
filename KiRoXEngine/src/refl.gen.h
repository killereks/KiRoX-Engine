#pragma once

#include <rttr/registration>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>
#include <sstream>

#include "Assets/Asset.h"
#include "Assets/Material.h"
#include "Assets/MeshFilter.h"
#include "Assets/Shader.h"
#include "Assets/Texture.h"
#include "Components/CameraComponent.h"
#include "Components/Colliders/BoxCollider.h"
#include "Components/Colliders/CapsuleCollider.h"
#include "Components/Colliders/Collider.h"
#include "Components/Component.h"
#include "Components/FirstPersonController.h"
#include "Components/Light/DirectionalLight.h"
#include "Components/Light/Light.h"
#include "Components/Light/SpotLight.h"
#include "Components/MeshComponent.h"
#include "Components/Rigidbody.h"
#include "Components/TestComponent.h"
#include "Components/TransformComponent.h"
#include "Components/Volume.h"
#include "Tools/ObjectPtr.h"
using namespace rttr;

inline std::string glm_vec2_to_string(const glm::vec2& vec, bool& ok){
std::stringstream ss;
ss << "[" << vec.x << ", " << vec.y << "]";
ok = true;
return ss.str();
}

inline std::string glm_vec3_to_string(const glm::vec3& vec, bool& ok){
std::stringstream ss;
ss << "[" << vec.x << ", " << vec.y << ", " << vec.z << "]";
ok = true;
return ss.str();
}

inline std::string glm_vec4_to_string(const glm::vec4& vec, bool& ok){
std::stringstream ss;
ss << "[" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << "]";
ok = true;
return ss.str();
}

inline std::string glm_quat_to_string(const glm::quat& quat, bool& ok){
std::stringstream ss;
ss << "[" << quat.x << ", " << quat.y << ", " << quat.z << ", " << quat.w << "]";
ok = true;
return ss.str();
}

RTTR_REGISTRATION { 
registration::class_<glm::vec2>("glm::vec2")
.constructor<>()
.property("x", &glm::vec2::x)
.property("y", &glm::vec2::y);
registration::class_<glm::vec3>("glm::vec3")
.constructor<>()
.property("x", &glm::vec3::x)
.property("y", &glm::vec3::y)
.property("z", &glm::vec3::z);
registration::class_<glm::vec4>("glm::vec4")
.constructor<>()
.property("x", &glm::vec4::x)
.property("y", &glm::vec4::y)
.property("z", &glm::vec4::z)
.property("w", &glm::vec4::w);
registration::class_<glm::quat>("glm::quat")
.constructor<>()
.property("x", &glm::quat::x)
.property("y", &glm::quat::y)
.property("z", &glm::quat::z)
.property("w", &glm::quat::w);
registration::class_<Asset>("Asset")
.property("uuid", &Asset::uuid)
;

registration::class_<Material>("Material")
.constructor<>()
.property("shader", &Material::shader)
.property("mainTexture", &Material::mainTexture)
.property("normalMap", &Material::normalMap)
.property("metallicMap", &Material::metallicMap)
.property("heightMap", &Material::heightMap)
.property("tiling", &Material::tiling)
.property("roughness", &Material::roughness)
;

registration::class_<MeshFilter>("MeshFilter")
.constructor<>()
;

registration::class_<Shader>("Shader")
.constructor<>()
;

registration::class_<Texture>("Texture")
.constructor<>()
;

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

registration::class_<BoxCollider>("BoxCollider")
.constructor<>()
.property("offset", &BoxCollider::offset)
.property("scale", &BoxCollider::scale)
;

registration::class_<CapsuleCollider>("CapsuleCollider")
.constructor<>()
.property("radius", &CapsuleCollider::radius)
.property("height", &CapsuleCollider::height)
;

registration::class_<Collider>("Collider")
.property("isTrigger", &Collider::isTrigger)
;

registration::class_<Component>("Component")
;

registration::class_<FirstPersonController>("FirstPersonController")
.constructor<>()
.property("speed", &FirstPersonController::speed)
.property("runningSpeed", &FirstPersonController::runningSpeed)
.property("sensitivity", &FirstPersonController::sensitivity)
;

registration::class_<DirectionalLight>("DirectionalLight")
.constructor<>()
;

registration::class_<Light>("Light")
;

registration::class_<SpotLight>("SpotLight")
.constructor<>()
.property("angle", &SpotLight::angle)
;

registration::class_<MeshComponent>("MeshComponent")
.constructor<>()
.property("mesh", &MeshComponent::mesh)
.property("material", &MeshComponent::material)
;

registration::class_<Rigidbody>("Rigidbody")
.constructor<>()
.property("isStatic", &Rigidbody::isStatic)
.property("freezeRotationX", &Rigidbody::freezeRotationX)
.property("freezeRotationY", &Rigidbody::freezeRotationY)
.property("freezeRotationZ", &Rigidbody::freezeRotationZ)
;

registration::class_<TestComponent>("TestComponent")
.constructor<>()
.property("a", &TestComponent::a)
.property("time", &TestComponent::time)
.property("name", &TestComponent::name)
.property("numbers", &TestComponent::numbers)
;

registration::class_<TransformComponent>("TransformComponent")
.constructor<>()
.property("position", &TransformComponent::position)
.property("rotation", &TransformComponent::rotation)
.property("scale", &TransformComponent::scale)
;

registration::class_<Volume>("Volume")
.constructor<>()
.property("testProcess", &Volume::testProcess)
;

registration::class_<ObjectPtr>("ObjectPtr")
.constructor<>()
.property("uuid", &ObjectPtr::uuid)
;

}

namespace Reflection {

inline CameraComponent* converter_CameraComponent(Component* comp, bool& ok){
ok = true;
return dynamic_cast<CameraComponent*>(comp);
}

inline BoxCollider* converter_BoxCollider(Component* comp, bool& ok){
ok = true;
return dynamic_cast<BoxCollider*>(comp);
}

inline CapsuleCollider* converter_CapsuleCollider(Component* comp, bool& ok){
ok = true;
return dynamic_cast<CapsuleCollider*>(comp);
}

inline Collider* converter_Collider(Component* comp, bool& ok){
ok = true;
return dynamic_cast<Collider*>(comp);
}

inline Component* converter_Component(Component* comp, bool& ok){
ok = true;
return dynamic_cast<Component*>(comp);
}

inline FirstPersonController* converter_FirstPersonController(Component* comp, bool& ok){
ok = true;
return dynamic_cast<FirstPersonController*>(comp);
}

inline DirectionalLight* converter_DirectionalLight(Component* comp, bool& ok){
ok = true;
return dynamic_cast<DirectionalLight*>(comp);
}

inline Light* converter_Light(Component* comp, bool& ok){
ok = true;
return dynamic_cast<Light*>(comp);
}

inline SpotLight* converter_SpotLight(Component* comp, bool& ok){
ok = true;
return dynamic_cast<SpotLight*>(comp);
}

inline MeshComponent* converter_MeshComponent(Component* comp, bool& ok){
ok = true;
return dynamic_cast<MeshComponent*>(comp);
}

inline Rigidbody* converter_Rigidbody(Component* comp, bool& ok){
ok = true;
return dynamic_cast<Rigidbody*>(comp);
}

inline TestComponent* converter_TestComponent(Component* comp, bool& ok){
ok = true;
return dynamic_cast<TestComponent*>(comp);
}

inline TransformComponent* converter_TransformComponent(Component* comp, bool& ok){
ok = true;
return dynamic_cast<TransformComponent*>(comp);
}

inline Volume* converter_Volume(Component* comp, bool& ok){
ok = true;
return dynamic_cast<Volume*>(comp);
}

static const rttr::type GetType(const std::string& name){
if (name == "Material"){
return rttr::type::get<Material*>();
}
if (name == "MeshFilter"){
return rttr::type::get<MeshFilter*>();
}
if (name == "Shader"){
return rttr::type::get<Shader*>();
}
if (name == "Texture"){
return rttr::type::get<Texture*>();
}
if (name == "CameraComponent"){
return rttr::type::get<CameraComponent*>();
}
if (name == "BoxCollider"){
return rttr::type::get<BoxCollider*>();
}
if (name == "CapsuleCollider"){
return rttr::type::get<CapsuleCollider*>();
}
if (name == "FirstPersonController"){
return rttr::type::get<FirstPersonController*>();
}
if (name == "DirectionalLight"){
return rttr::type::get<DirectionalLight*>();
}
if (name == "SpotLight"){
return rttr::type::get<SpotLight*>();
}
if (name == "MeshComponent"){
return rttr::type::get<MeshComponent*>();
}
if (name == "Rigidbody"){
return rttr::type::get<Rigidbody*>();
}
if (name == "TestComponent"){
return rttr::type::get<TestComponent*>();
}
if (name == "TransformComponent"){
return rttr::type::get<TransformComponent*>();
}
if (name == "Volume"){
return rttr::type::get<Volume*>();
}
if (name == "ObjectPtr"){
return rttr::type::get<ObjectPtr*>();
}
std::cout << "No type found for " << name << "\n";
return rttr::type::get<void>();
}

static Component* CreateComponent(const std::string& name){
if (name == "CameraComponent"){
return new CameraComponent();
}
if (name == "BoxCollider"){
return new BoxCollider();
}
if (name == "CapsuleCollider"){
return new CapsuleCollider();
}
if (name == "FirstPersonController"){
return new FirstPersonController();
}
if (name == "DirectionalLight"){
return new DirectionalLight();
}
if (name == "SpotLight"){
return new SpotLight();
}
if (name == "MeshComponent"){
return new MeshComponent();
}
if (name == "Rigidbody"){
return new Rigidbody();
}
if (name == "TestComponent"){
return new TestComponent();
}
if (name == "TransformComponent"){
return new TransformComponent();
}
if (name == "Volume"){
return new Volume();
}
throw std::runtime_error("No type found for " + name);
}

static void RegisterTypes(){
rttr::type::get<Component*>().register_converter_func(converter_CameraComponent);
rttr::type::get<Component*>().register_converter_func(converter_BoxCollider);
rttr::type::get<Component*>().register_converter_func(converter_CapsuleCollider);
rttr::type::get<Component*>().register_converter_func(converter_FirstPersonController);
rttr::type::get<Component*>().register_converter_func(converter_DirectionalLight);
rttr::type::get<Component*>().register_converter_func(converter_SpotLight);
rttr::type::get<Component*>().register_converter_func(converter_MeshComponent);
rttr::type::get<Component*>().register_converter_func(converter_Rigidbody);
rttr::type::get<Component*>().register_converter_func(converter_TestComponent);
rttr::type::get<Component*>().register_converter_func(converter_TransformComponent);
rttr::type::get<Component*>().register_converter_func(converter_Volume);
rttr::type::get<glm::vec2>().register_converter_func(glm_vec2_to_string);
rttr::type::get<glm::vec3>().register_converter_func(glm_vec3_to_string);
rttr::type::get<glm::vec4>().register_converter_func(glm_vec4_to_string);
rttr::type::get<glm::quat>().register_converter_func(glm_quat_to_string);
}

static std::vector<rttr::type> GetAllNonAbstractComponents(){
std::vector<rttr::type> types;
types.push_back(rttr::type::get<CameraComponent>());
types.push_back(rttr::type::get<BoxCollider>());
types.push_back(rttr::type::get<CapsuleCollider>());
types.push_back(rttr::type::get<FirstPersonController>());
types.push_back(rttr::type::get<DirectionalLight>());
types.push_back(rttr::type::get<SpotLight>());
types.push_back(rttr::type::get<MeshComponent>());
types.push_back(rttr::type::get<Rigidbody>());
types.push_back(rttr::type::get<TestComponent>());
types.push_back(rttr::type::get<TransformComponent>());
types.push_back(rttr::type::get<Volume>());
return types;
}

}

