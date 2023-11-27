#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "../Editor/Console.h"

#include "Asset.h"

class Shader : public Asset {
public:
	unsigned int ID;
	// TODO: Use Hashing to store the uniform locations
	// TODO: Convert this into an asset
	
	// constructor generates the shader on the fly
	// ------------------------------------------------------------------------
	std::string GetSrc(const std::string& filePath, const std::string token)
	{
		bool isToken = false;

		std::string outCode;
		std::ifstream shaderFile(filePath);

		std::string currentLine;
		while (std::getline(shaderFile, currentLine))
		{
			if (currentLine == token)
			{
				isToken = true;
				continue;
			}

			if (isToken)
			{
				if (currentLine == "@end")
				{
					break;
				}
				outCode += currentLine+"\n";
			}
		}

		return outCode;
	}

	Shader(const std::string& filePath)
	{
		Console::Write("Reading shaders from path "+filePath, ImVec4(0.322, 0.761, 0.831, 1.0));

		std::string vertexCode = GetSrc(filePath, "@vs");
		std::string fragmentCode = GetSrc(filePath, "@fs");

		std::cout << "Vertex\n" << vertexCode;
		std::cout << "\nFragment\n" << fragmentCode;

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();
		// 2. compile shaders
		unsigned int vertex, fragment;

		// vertex shader
		Console::Write("Compiling vertex shader...", ImVec4(0.8, 0.922, 0.373, 1.0));
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");

		// fragment Shader
		Console::Write("Compiling fragment shader...", ImVec4(0.8, 0.922, 0.373, 1.0));
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");

		// shader Program
		ID = glCreateProgram();

		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);

		glLinkProgram(ID);

		checkCompileErrors(ID, "PROGRAM");
		// delete the shaders as they're linked into our program now and no longer necessary
		glDeleteShader(vertex);
		glDeleteShader(fragment);

		Console::Write("Successfully compiled...", ImVec4(0.322, 0.859, 0.302, 1.0));

		this->loaded = true;
	}

	// activate the shader
	// ------------------------------------------------------------------------
	void use()
	{
		glUseProgram(ID);
	}
	// utility uniform functions
	// ------------------------------------------------------------------------
	void setBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	// ------------------------------------------------------------------------
	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void setVec2(const std::string& name, const glm::vec2& value) const
	{
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec2(const std::string& name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}
	// ------------------------------------------------------------------------
	void setVec3(const std::string& name, const glm::vec3& value) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec3(const std::string& name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}
	// ------------------------------------------------------------------------
	void setVec4(const std::string& name, const glm::vec4& value) const
	{
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec4(const std::string& name, float x, float y, float z, float w)
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}
	// ------------------------------------------------------------------------
	void setMat2(const std::string& name, const glm::mat2& mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat3(const std::string& name, const glm::mat3& mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat4(const std::string& name, const glm::mat4& mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	int getUniformLocation(const std::string& name) const
	{
		return glGetUniformLocation(ID, name.c_str());
	}

	void setVec3(int location, const glm::vec3& value) const
	{
		glUniform3fv(location, 1, &value[0]);
	}

	void setFloat(int location, float value) const
	{
		glUniform1f(location, value);
	}

	void setInt(int location, int value) const
	{
		glUniform1i(location, value);
	}

	void setMat4(int location, const glm::mat4& value) const {
		glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
	}
	

private:
	// utility function for checking shader compilation/linking errors.
	// ------------------------------------------------------------------------
	void checkCompileErrors(GLuint shader, std::string type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				Console::Write("ERROR::SHADER_COMPILATION_ERROR of type: "+type, ImVec4(1.0, 0.0, 0.0, 1.0));
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				Console::Write("ERROR::PROGRAM_LINKING_ERROR of type: "+type, ImVec4(1.0, 0.0, 0.0, 1.0));
			}
		}
	}
};
#endif