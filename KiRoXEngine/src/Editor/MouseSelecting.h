#pragma once
#include <vector>
#include <Entity.h>
#include <glm/glm.hpp>
#include <Components/CameraComponent.h>

class MouseSelecting
{
	RenderTexture* renderTexture;

public:
	MouseSelecting();
	~MouseSelecting();

	unsigned int GetPickTexture() { return renderTexture->GetTextureID(); }
	unsigned int GetDepthTexture() { return renderTexture->GetDepthTextureID(); }

	RenderTexture* GetRenderTexture() { return renderTexture; }

	void Resize(unsigned int width, unsigned int height);

	glm::vec3 MouseClicked(std::vector<MeshComponent*>& meshes, CameraComponent* sceneCamera, glm::vec2 mousePos);
};

