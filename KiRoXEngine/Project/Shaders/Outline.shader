@vs
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);
}
@end

@fs
#version 460 core

out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0, 0.5, 0.0, 1.0);
}
@end