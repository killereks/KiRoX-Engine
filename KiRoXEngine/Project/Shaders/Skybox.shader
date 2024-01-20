@vs
#version 460 core

layout (location = 0) in vec3 aPos;

out vec3 texCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;

    texCoords = vec3(aPos.x, aPos.y, -aPos.z);
}

@end
@fs

in vec3 texCoords;

out vec4 FragColor;

uniform samplerCube skybox;

void main(){
    FragColor = texture(skybox, texCoords);
}

@end