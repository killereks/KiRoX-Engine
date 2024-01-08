#include "RenderTexture.h"

#include "GL/glew.h"
#include <string>
#include "../Editor/Console.h"
#include <iostream>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "image/stb_image_write.h"

#include <fstream>

void CheckOpenGLError(const char* stmt, const char* fname, int line)
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		printf("OpenGL error %08x, at %s:%i - for %s\n", err, fname, line, stmt);
		abort();
	}
}

#ifdef _DEBUG
#define GL_CHECK(stmt) do { \
            stmt; \
            CheckOpenGLError(#stmt, __FILE__, __LINE__); \
        } while (0)
#else
#define GL_CHECK(stmt) stmt
#endif

RenderTexture::RenderTexture(int width, int height)
{
	Resize(width, height);
}

glm::vec3 RenderTexture::GetPixel(int x, int y)
{
	if (x < 0 || x > width || y < 0 || y > height) return glm::vec3(0.0f);

	glm::vec3 pixel = glm::vec3(0.0f);

	y = height - y;

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	unsigned int* pixels = new unsigned int[3];
	glReadPixels(x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, pixels);

	pixel = glm::vec3(pixels[0], pixels[1], pixels[2]);

	delete[] pixels;

	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return pixel;
}

void RenderTexture::Bind()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void RenderTexture::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderTexture::Resize(int width, int height)
{
	if (this->width == width && this->height == height) return;

	if (FBO != 0)
	{
		glDeleteFramebuffers(1, &FBO);
	}

	glGenFramebuffers(1, &FBO);

	this->width = width;
	this->height = height;

	// FRAME BUFFER
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// COLOR TEXTURE
	glGenTextures(1, &colorTexture);
	glBindTexture(GL_TEXTURE_2D, colorTexture);

	if (useInts) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, width, height, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

	// DEPTH TEXTURE
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Console::Write("Something went wrong with the frame buffer! " + std::to_string(glCheckFramebufferStatus(GL_FRAMEBUFFER)), ImVec4(1.0, 0.0, 0.0, 1.0));
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderTexture::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

RenderTexture::~RenderTexture()
{
	glDeleteFramebuffers(1, &FBO);
	glDeleteTextures(1, &colorTexture);
	glDeleteTextures(1, &depthTexture);
}
