#pragma once
#include <vector>
#include <Components/MeshComponent.h>

#include <Assets/Shader.h>
#include "glm/glm.hpp"

class ShadowMap
{
	unsigned int depthMapFBO;
	unsigned int depthMap;

	unsigned int width;
	unsigned int height;

	Shader* shader;

public:
	ShadowMap(unsigned int width, unsigned int height);
	~ShadowMap();

	unsigned int GetDepthMap() { return depthMap; }

	void Resize(unsigned int width, unsigned int height);

	void Render(std::vector<MeshComponent*>& meshes, glm::mat4 lightMatrix);
	void Render(MeshComponent* mesh, glm::mat4 lightMatrix);
};

