#pragma once

#include <Components/Component.h>
#include <Rendering/ShadowMap.h>

CLASS()
class DirectionalLight : public Component {
	REFLECT(Component)

	ShadowMap* shadowMap;

public:
	virtual void DrawInspector() override;

	virtual void OnDrawGizmos() override;

	virtual std::string GetIcon() override;

	unsigned int GetShadowMap(){ return shadowMap->GetDepthMap(); }

	void Render(std::vector<MeshComponent*>& meshes);

	void BindShadowMap(int slot);

	glm::mat4 GetLightSpaceMatrix();

	DirectionalLight();
	~DirectionalLight();
};

