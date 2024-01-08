@vs
#version 460 core

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inTexCoords;

out vec2 texCoords;

void main(){
    gl_Position = vec4(inPos.x, inPos.y, 0.0, 1.0);
    texCoords = inTexCoords;
}
@end
@fs
#version 460 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D screenTexture;

void main(){
    FragColor = 1.0 - texture(screenTexture, texCoords);
}


@end