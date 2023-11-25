#pragma once
#include "Asset.h"
#include <string>

class Texture : public Asset
{
private:
	int width, height;

	unsigned int textureID;

public:
	Texture(const std::string& filePath);
	~Texture();

	unsigned int GetTextureID() const { return textureID; }

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const { return width; }
	inline int GetHeight() const { return height; }
};

