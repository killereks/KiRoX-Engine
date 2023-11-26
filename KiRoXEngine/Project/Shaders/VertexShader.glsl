#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 perspectiveMatrix;

out vec3 vertexPos;

void main(){
    gl_Position = perspectiveMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);

    vertexPos = aPos;
}