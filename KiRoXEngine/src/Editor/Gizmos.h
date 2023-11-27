#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "GL/glew.h"
#include "../Assets/Shader.h"

class Gizmos
{
	static Gizmos* instance;

	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;

	unsigned int VAO;
	unsigned int VBO;
	unsigned int IBO;
	unsigned int CBO;

	Shader* shader;

public:

	static Gizmos* GetInstance()
	{
		if (instance == nullptr)
		{
			instance = new Gizmos();
		}

		return instance;
	}

	void Init(Shader* shader)
	{
		glGenVertexArrays(1, &VAO);

		glGenBuffers(1, &VBO);
		glGenBuffers(1, &IBO);
		glGenBuffers(1, &CBO);

		this->shader = shader;
	}

	~Gizmos()
	{
		glDeleteVertexArrays(1, &VAO);

		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &IBO);
		glDeleteBuffers(1, &CBO);
	}

	void Draw()
	{
		if (vertices.size() == 0) return;

		shader->use();

		// draw
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glBindVertexArray(VAO);

		// VERTICES
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

		//// COLORS
		//glBindBuffer(GL_ARRAY_BUFFER, CBO);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(colors), &colors[0], GL_DYNAMIC_DRAW);
		//
		//glEnableVertexAttribArray(1);
		//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_DYNAMIC_DRAW);

		glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);

		vertices.clear();
		indices.clear();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	static void DrawWireCube(glm::vec3 position, glm::vec3 size, glm::vec3 color);
	static void DrawLine(glm::vec3 startPosition, glm::vec3 endPosition, glm::vec3 color);
	static void DrawRay(glm::vec3 startPosition, glm::vec3 direction, glm::vec3 color);
	static void DrawWireSphere(glm::vec3 position, float radius, glm::vec3 color);
};

