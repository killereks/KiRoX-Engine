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
    float tiling = 30.0;

    float x = floor(vertexPos.x * tiling);
    float y = floor(vertexPos.y * tiling);
    float result = sign(mod(x + y, 2.0));

    if (result < 1.0) {
        FragColor = vec4(0.5, 0.5, 0.5, 1.0);
    } else {
        FragColor = vec4(0.8, 0.8, 0.8, 1.0);
    }
}
@end