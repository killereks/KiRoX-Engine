#pragma once

#include <GL/glew.h>
#include <Assets/Shader.h>

class RenderTools
{
    static constexpr float quadVertices[] = {
        // positions
        -1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f, -1.0f,

        -1.0f,  1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
    };

    static unsigned int quadVAO, quadVBO;

public:
    static void Initialize();
    static void Destroy();

    static void DrawScreenQuad(unsigned int texture, Shader* shader);
};
