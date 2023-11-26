#version 330 core

in vec3 vertexPos;

out vec4 FragColor;

void main(){
    float tiling = 30.0;

    float x = floor(vertexPos.x * tiling);
    float y = floor(vertexPos.y * tiling);
    float result = sign(mod(x + y, 2.0));

    if (result < 1.0) {
        FragColor = vec4(0.2, 0.2, 0.2, 1.0);
    } else {
        FragColor = vec4(0.5, 0.5, 0.5, 1.0);
    }
}