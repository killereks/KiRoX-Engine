#include "ComputeShader.h"

#include <fstream>
#include <GL/glew.h>

void ComputeShader::CreateBuffer(unsigned int index)
{
	if (buffers.size() <= index)
	{
		buffers.resize(index + 1);
	}

	glGenBuffers(1, &buffers[index]);
}

void ComputeShader::CheckCompileErrors(unsigned int shader, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}

ComputeShader::ComputeShader()
{
	
}

ComputeShader::~ComputeShader()
{
	glDeleteShader(computeShaderID);

	for (unsigned int i = 0; i < buffers.size(); i++)
	{
		glDeleteBuffers(1, &buffers[i]);
	}
}

void ComputeShader::Bind()
{
	glUseProgram(computeShaderID);
}

void ComputeShader::Unbind()
{
	glUseProgram(0);
}

void ComputeShader::Recompile()
{
	LoadShader();
}

void ComputeShader::Dispatch(unsigned int x, unsigned int y, unsigned int z)
{
	glDispatchCompute(x, y, z);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void ComputeShader::SetData(unsigned int index, void* data, unsigned int size)
{
	if (buffers.size() <= index)
	{
		CreateBuffer(index);
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[index]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, buffers[index]);
}

void ComputeShader::SetBool(const std::string& name, bool value) const
{
	glUniform1d(glGetUniformLocation(computeShaderID, name.c_str()), (int)value);
}

void ComputeShader::SetInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(computeShaderID, name.c_str()), value);
}

void ComputeShader::SetFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(computeShaderID, name.c_str()), value);
}

void ComputeShader::LoadShader()
{
	std::ifstream shaderFile;

	shaderFile.open(filePath);

	std::string shaderCode;

	std::string currentLine;

	while (std::getline(shaderFile, currentLine))
	{
		shaderCode += currentLine + "\n";
	}

	const char* shaderCodeCStr = shaderCode.c_str();

	unsigned int compute;

	compute = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(compute, 1, &shaderCodeCStr, NULL);
	glCompileShader(compute);
	CheckCompileErrors(compute, "COMPUTE_SHADER");

	computeShaderID = glCreateProgram();
	glAttachShader(computeShaderID, compute);
	glLinkProgram(computeShaderID);
	CheckCompileErrors(computeShaderID, "PROGRAM");

	glDeleteShader(compute);

	shaderFile.close();

	loaded = true;
}

void ComputeShader::SetVec2(const std::string& name, const glm::vec2& value) const
{
	glUniform2fv(glGetUniformLocation(computeShaderID, name.c_str()), 1, &value[0]);
}

void ComputeShader::SetTexture(unsigned int textureID, unsigned int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void ComputeShader::SetTextureLayout(unsigned int textureID, unsigned int slot)
{
	glBindImageTexture(slot, textureID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}

co::Coro ComputeShader::BeginLoading()
{
	LoadShader();
	return {};
}