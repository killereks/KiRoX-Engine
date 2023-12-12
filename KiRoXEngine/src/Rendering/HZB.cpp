#include "HZB.h"

HZB::HZB(unsigned int width, unsigned int height, unsigned int mipLevels)
{
	Resize(width, height);
}

HZB::~HZB()
{
	glDeleteFramebuffers(1, &FBO);
	glDeleteTextures(1, &depthTexture);
}

void HZB::Render(std::vector<MeshComponent*>& meshes)
{
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;


	int currentIdx = 0;
	// gather all bounding boxes and batch them
	for (auto& mesh : meshes)
	{
		Bounds* bounds = mesh->GetBounds();
		if (bounds == nullptr) continue;

		std::vector<unsigned int> localIndices;

		bounds->GetRenderCube(vertices, localIndices);

		for (auto& idx : localIndices)
		{
			indices.emplace_back(idx + currentIdx);
		}

		currentIdx += localIndices.size();
	}

	mipLevels = 1 + (int)floor(log2(std::max(width, height)));

	int currentWidth = width;
	int currentHeight = height;

	for (int i = 1; i < mipLevels; i++) {
		currentWidth /= 2;
		currentHeight /= 2;

		currentWidth = currentWidth > 0 ? currentWidth : 1;
		currentHeight = currentHeight > 0 ? currentHeight : 1;

		glViewport(0, 0, currentWidth, currentHeight);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, i - 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, i - 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, i);

		// here we render
		glClear(GL_DEPTH_BUFFER_BIT);

		glEnableVertexAttribArray(0);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices.data());

		glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, indices.data());
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipLevels - 1);
}

void HZB::Resize(unsigned int width, unsigned int height)
{
	if (FBO != 0)
	{
		glDeleteFramebuffers(1, &FBO);
		glDeleteTextures(1, &depthTexture);
	}

	this->width = width;
	this->height = height;

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &depthTexture);

	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
