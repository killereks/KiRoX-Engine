#pragma once
#include <Components/Component.h>
#include <Macros.h>

#include <Rendering/ShadowMap.h>

CLASS()
class Light : public Component
{
	REFLECT(Component)

protected:
	ShadowMap* shadowMap;

public:
	Light();
	~Light();

	virtual std::string GetIcon() = 0;

	unsigned int GetShadowMap() { return shadowMap->GetDepthMap(); }

	void Render(std::vector<MeshComponent*>& meshes);
	void Render(MeshComponent* mesh);

	void BindShadowMap(int slot);

	virtual glm::mat4 GetLightSpaceMatrix() = 0;

};

