#include "RenderTexture.h"

#include "GL/glew.h"
#include <string>
#include "../Editor/Console.h"

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

void RenderTexture::Bind()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	Clear();
}

void RenderTexture::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderTexture::Resize(int width, int height)
{
	if (this->width == width && this->height == height) return;

	Console::Write("Window size changed from " + std::to_string(this->width) + "x" + std::to_string(this->height) + " to " + std::to_string(width) + "x" + std::to_string(height));

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

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, colorTexture);

	// DEPTH TEXTURE
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// ATTACH TEXTURE TO FRAMEBUFFER
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Console::Write("Something went wrong with the frame buffer! " + std::to_string(glCheckFramebufferStatus(GL_FRAMEBUFFER)), ImVec4(1.0, 0.0, 0.0, 1.0));
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
