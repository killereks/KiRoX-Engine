#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "../Editor/Console.h"

#include <unordered_map>

#include "Asset.h"

class Shader : public Asset {
public:
	unsigned int ID;
	// TODO: Use Hashing to store the uniform locations

	std::unordered_map<std::string, unsigned int> locationCache;
	
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
		Console::Write("Reading shaders from path "+filePath, ImVec4(0.322f, 0.761f, 0.831f, 1.0f));

		std::string vertexCode = GetSrc(filePath, "@vs");
		std::string fragmentCode = GetSrc(filePath, "@fs");

		std::cout << "Vertex\n" << vertexCode;
		std::cout << "\nFragment\n" << fragmentCode;

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();
		// 2. compile shaders
		unsigned int vertex, fragment;

		// vertex shader
		Console::Write("Compiling vertex shader...", ImVec4(0.8f, 0.922f, 0.373f, 1.0f));
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");

		// fragment Shader
		Console::Write("Compiling fragment shader...", ImVec4(0.8f, 0.922f, 0.373f, 1.0f));
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

		Console::Write("Successfully compiled...", ImVec4(0.322f, 0.859f, 0.302f, 1.0f));

		this->loaded = true;
	}

	// activate the shader
	// ------------------------------------------------------------------------
	void use()
	{
		glUseProgram(ID);
	}

	unsigned int getLocation(const std::string& name)
	{
		if (locationCache.find(name) != locationCache.end())
		{
			unsigned int location = glGetUniformLocation(ID, name.c_str());
			locationCache[name] = location;
		}

		return locationCache[name];
	}

	// utility uniform functions
	// ------------------------------------------------------------------------
	void setBool(const std::string& name, bool value)
	{
		glUniform1i(getLocation(name), (int)value);
	}
	// ------------------------------------------------------------------------
	void setInt(const std::string& name, int value)
	{
		glUniform1i(getLocation(name), value);
	}
	// ------------------------------------------------------------------------
	void setFloat(const std::string& name, float value)
	{
		glUniform1f(getLocation(name), value);
	}
	// ------------------------------------------------------------------------
	void setVec2(const std::string& name, const glm::vec2& value)
	{
		glUniform2fv(getLocation(name), 1, &value[0]);
	}
	void setVec2(const std::string& name, float x, float y)
	{
		glUniform2f(getLocation(name), x, y);
	}
	// ------------------------------------------------------------------------
	void setVec3(const std::string& name, const glm::vec3& value)
	{
		glUniform3fv(getLocation(name), 1, &value[0]);
	}
	void setVec3(const std::string& name, float x, float y, float z)
	{
		glUniform3f(getLocation(name), x, y, z);
	}
	// ------------------------------------------------------------------------
	void setVec4(const std::string& name, const glm::vec4& value)
	{
		glUniform4fv(getLocation(name), 1, &value[0]);
	}
	void setVec4(const std::string& name, float x, float y, float z, float w)
	{
		glUniform4f(getLocation(name), x, y, z, w);
	}
	// ------------------------------------------------------------------------
	void setMat2(const std::string& name, const glm::mat2& mat)
	{
		glUniformMatrix2fv(getLocation(name), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat3(const std::string& name, const glm::mat3& mat)
	{
		glUniformMatrix3fv(getLocation(name), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat4(const std::string& name, const glm::mat4& mat)
	{
		glUniformMatrix4fv(getLocation(name), 1, GL_FALSE, &mat[0][0]);
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