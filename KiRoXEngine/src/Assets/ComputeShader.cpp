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

ComputeShader::ComputeShader()
{
	for (unsigned int i = 0; i < buffers.size(); i++)
	{
		glDeleteBuffers(1, &buffers[i]);
	}
}

ComputeShader::~ComputeShader()
{
	glDeleteShader(computeShaderID);
}

void ComputeShader::Dispatch(unsigned int x, unsigned int y, unsigned int z)
{
	glUseProgram(computeShaderID);
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

	computeShaderID = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(computeShaderID, 1, &shaderCodeCStr, NULL);
	glCompileShader(computeShaderID);

	int success;

	glGetShaderiv(computeShaderID, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(computeShaderID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::COMPUTE SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;

		loaded = true;

		shaderFile.close();
		return;
	}

	unsigned int computeProgram = glCreateProgram();
	glAttachShader(computeProgram, computeShaderID);
	glLinkProgram(computeProgram);

	glGetProgramiv(computeProgram, GL_LINK_STATUS, &success);

	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(computeProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::COMPUTE SHADER::LINKING_FAILED\n" << infoLog << std::endl;

		loaded = true;

		shaderFile.close();
		return;
	}

	loaded = true;

	shaderFile.close();
}

co::Coro ComputeShader::BeginLoading()
{
	LoadShader();
	return {};
}
