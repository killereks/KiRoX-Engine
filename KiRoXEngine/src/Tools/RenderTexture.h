#pragma once

#include <glm/glm.hpp>
#include <string>

class RenderTexture
{
	int width = 0;
	int height = 0;

	bool useInts = false;

	unsigned int FBO;

	unsigned int colorTexture;
	unsigned int depthTexture;

public:
	RenderTexture(int width, int height);

	void UseInts(bool useInts) { this->useInts = useInts; }

	int GetWidth() const { return width; }
	int GetHeight() const { return height; }

	int GetFBO() const { return FBO; }

	const unsigned int GetTextureID() const { return colorTexture; }
	const unsigned int GetDepthTextureID() const { return depthTexture; }

	glm::vec3 GetPixel(int x, int y);

	void Bind();
	void Unbind();

	void Resize(int width, int height);

	void Clear();
	void ClearColor(glm::vec4 color);

	~RenderTexture();
};

