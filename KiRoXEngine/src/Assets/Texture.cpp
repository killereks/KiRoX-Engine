#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "image_load/stb_image.h"

#include <GL/glew.h>
#include <iostream>

Texture::Texture(const std::string& filePath)
{
	int width;
	int height;
	int comp;

	stbi_uc* pixels = stbi_load(filePath.c_str(), &width, &height, &comp, 4);

	if (pixels == nullptr) {
		std::cout << "Failed to load image: " << filePath << std::endl;
		return;
	}

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_LINEAR
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	stbi_image_free(pixels);

	this->loaded = true;
}

Texture::~Texture()
{
	glDeleteTextures(1, &textureID);
}
