#pragma once

#include <Components/Component.h>
#include <Rendering/ShadowMap.h>
#include <Components/Light/Light.h>

CLASS()
class DirectionalLight : public Light {
	REFLECT(Light)

public:
	virtual bool DrawInspector() override;

	virtual void OnDrawGizmos() override;

	virtual std::string GetIcon() override;

	virtual glm::mat4 GetLightSpaceMatrix() override;
	glm::mat4 GetTightLightSpaceMatrix(CameraComponent* camera);

	void RenderTightCamera(std::vector<MeshComponent*>& meshes, CameraComponent* camera);

	DirectionalLight();
	~DirectionalLight();
};

