@vs
#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 perspectiveMatrix;

out vec3 vertexPos;

void main(){
    gl_Position = perspectiveMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);

    vertexPos = aPos;
}

@end

@fs
#version 460 core

in vec3 vertexPos;

out vec4 FragColor;

void main(){
    FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}
@end