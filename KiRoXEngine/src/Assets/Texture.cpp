#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "image_load/stb_image.h"

#include "../Tools/File.h"

#include <thread>

#include <GL/glew.h>
#include <iostream>

Texture::Texture()
{
	
}

Texture::~Texture()
{
	glDeleteTextures(1, &textureID);
}

void Texture::Bind(unsigned int slot /*= 0*/) const
{
	if (loaded)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, textureID);
	}
}

void Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

co::Coro Texture::BeginLoading()
{
	co::Coro coro;
	coro.init([]()
		{
			Texture* self = (Texture*)co::getUserData();

			std::thread loadThread([self]()
				{
					self->Load();
				});

			loadThread.detach();

			while (self->pixels == nullptr)
			{
				co::yield();
			}

			self->UploadToGPU();

			//stbi_image_free(self->pixels);

		}, this);

	return coro;
}

void Texture::Load()
{
	pixels = stbi_load(filePath.c_str(), &width, &height, &comp, 4);

	if (pixels == nullptr)
	{
		std::cout << "Failed to load image: " << filePath << std::endl;
	}
}

void Texture::UploadToGPU()
{
	if (pixels == nullptr)
	{
		std::cerr << "Error, pixels are null for " << filePath << "\n";
		return;
	}

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	hasLoaded.store(true);
}
