#include "ComputeShader.h"

#include <fstream>
#include <GL/glew.h>

ComputeShader::ComputeShader()
{
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
