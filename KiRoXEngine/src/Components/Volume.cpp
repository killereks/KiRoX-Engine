#include "Volume.h"
#include <Rendering/RenderTools.h>
#include <chrono>

void Volume::SetupFBO(int width, int height)
{
	if (this->width == width && this->height == height) return;

	this->width = width;
	this->height = height;

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// COLOR BUFFER
	glGenTextures(1, &textureColorBuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

	// RBO
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
}

void Volume::Draw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glBindVertexArray(quadVAO);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Volume::Volume()
{
	testProcess = new ObjectPtr();
}

Volume::~Volume()
{
	glDeleteFramebuffers(1, &FBO);
	glDeleteRenderbuffers(1, &RBO);

	glDeleteTextures(1, &textureColorBuffer);

	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
}

void Volume::Apply(unsigned int textureID, int width, int height, CameraComponent* camera, DirectionalLight* light)
{
	if (!testProcess->HasValue()) return;

	if (camera == nullptr) return;
	if (light == nullptr) return;

	/*
	FBO Ping Pong Technique:
	for each process
		Bind framebuffer
		Render a screen quad with the texture
	
	*/

	Shader* shader = testProcess->Get<Shader>();

	if (shader == nullptr) return;

	shader->use();

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, light->GetShadowMap());
	shader->setInt("shadowMap", 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, camera->GetRenderTextureDepthID());
	shader->setInt("cameraDepthMap", 2);

	// lights
	shader->setMat4("lightSpaceMatrix", light->GetLightSpaceMatrix());

	// camera
	shader->setVec3("camPos", camera->GetOwner()->GetTransform().GetWorldPosition());
	shader->setVec3("camFwd", -camera->GetOwner()->GetTransform().GetForward());
	shader->setVec3("camUp", camera->GetOwner()->GetTransform().GetUp());
	shader->setVec3("camRight", camera->GetOwner()->GetTransform().GetRight());

	shader->setFloat("nearPlane", camera->GetNearClipPlane());
	shader->setFloat("farPlane", camera->GetFarClipPlane());

	shader->setMat4("projection", camera->GetProjectionMatrix());
	shader->setMat4("view", camera->GetViewMatrix());

	float time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() / 1000.0f;
	shader->setFloat("time", time);

	//SetupFBO(width, height);

	RenderTools::DrawScreenQuad(textureID, shader);
}
