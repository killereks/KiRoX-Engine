#pragma once

#include <GL/glew.h>
#include <vector>
#include <Components/Light/SpotLight.h>
#include <Components/MeshComponent.h>

struct SpotLightData {
	glm::vec3 pos;
	glm::vec3 dir;
	float angle;
};

class SceneShadowMapper
{
	unsigned int shadowMapArray;
	unsigned int shadowMapFBO;

	Shader* shadowShader;

	int maxLights = 16;

	int currentDrawIndex = 0;

	std::vector<glm::mat4> lightSpaceMatrices;
	std::vector<SpotLightData> spotLightData;

public:
	SceneShadowMapper();
	~SceneShadowMapper();
	
	void DrawDebug();

	void BeginShadowMapping();
	void RenderSpotLight(SpotLight* spotLight, std::vector<MeshComponent*>& meshComponents);
	void EndShadowMapping();

	void BindShadowMapArray(Shader* shader);
};

