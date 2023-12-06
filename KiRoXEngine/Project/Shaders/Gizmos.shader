@vs
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 perspectiveMatrix;

out vec3 vertexColor;

void main(){
    gl_Position = perspectiveMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);

	vertexColor = aColor;
}

@end

@fs
#version 460 core

in vec3 vertexColor;

out vec4 FragColor;

void main(){
    FragColor = vec4(vertexColor, 1.0);
}
@end