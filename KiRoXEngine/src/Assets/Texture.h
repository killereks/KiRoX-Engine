#pragma once
#include "Asset.h"
#include <string>

#include <atomic>

#include "image_load/stb_image.h"

class Texture : public Asset
{
private:
	int width, height, comp;

	unsigned int textureID;

	std::atomic<bool> hasLoaded{ false };

	stbi_uc* pixels;

public:
	Texture();
	~Texture();

	unsigned int GetTextureID() const { return textureID; }

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	void Load();
	void UploadToGPU();

	virtual co::Coro BeginLoading() override;

	inline int GetWidth() const { return width; }
	inline int GetHeight() const { return height; }
};

