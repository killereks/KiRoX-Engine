#pragma once
#include <vector>
#include "../Components/MeshComponent.h"
class HZB
{
	unsigned int FBO;
	unsigned int depthTexture;

	unsigned int width;
	unsigned int height;

	unsigned int mipLevels;

	public:
		HZB(unsigned int width, unsigned int height, unsigned int mipLevels);
		~HZB();

		void Render(std::vector<MeshComponent*>& meshes);

		void Resize(unsigned int width, unsigned int height);

		const unsigned int GetTextureID() const { return depthTexture; }
		const unsigned int GetMipLevels() const { return mipLevels; }
};

