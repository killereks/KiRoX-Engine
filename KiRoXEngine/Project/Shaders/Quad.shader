@vs
#version 460 core

layout (location = 0) in vec3 aPos;

out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aPos.xy * 0.5 + 0.5;
}
@end
@fs
#version 460 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D screenTexture;

void main(){
    FragColor = texture(screenTexture, TexCoord);
}
@end