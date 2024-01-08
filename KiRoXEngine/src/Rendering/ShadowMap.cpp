#include "ShadowMap.h"

#include <GL/glew.h>

#include <Assets/AssetManager.h>

ShadowMap::ShadowMap(unsigned int width, unsigned int height)
{
	Resize(width, height);

	shader = AssetManager::GetInstance()->Get<Shader>("Shadow.shader");
}

ShadowMap::~ShadowMap()
{
	glDeleteFramebuffers(1, &depthMapFBO);
	glDeleteTextures(1, &depthMap);
}

void ShadowMap::Resize(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;

	if (depthMapFBO != 0)
	{
		glDeleteFramebuffers(1, &depthMapFBO);
	}

	if (depthMap != 0)
	{
		glDeleteTextures(1, &depthMap);
	}

	// FRAMEBUFFER
	glGenFramebuffers(1, &depthMapFBO);

	// TEXTURE
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

	// border color
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// check for errors
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "[ShadowMap] Framebuffer not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::Render(std::vector<MeshComponent*>& meshes, glm::mat4 lightMatrix)
{
	shader->use();

	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

	glClear(GL_DEPTH_BUFFER_BIT);

	shader->setMat4("lightSpaceMatrix", lightMatrix);

	for (auto& mesh : meshes)
	{
		mesh->SimpleDraw(shader);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
