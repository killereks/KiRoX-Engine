#pragma once
class RenderTexture
{
	int width = 0;
	int height = 0;

	unsigned int FBO;

	unsigned int colorTexture;
	unsigned int depthTexture;

public:
	RenderTexture(int width, int height);

	const unsigned int GetTextureID() const { return colorTexture; }
	const unsigned int GetDepthTextureID() const { return depthTexture; }

	void Bind();
	void Unbind();

	void Resize(int width, int height);

	void Clear();

	~RenderTexture();
};

