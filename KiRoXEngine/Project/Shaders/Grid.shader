@vs
#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 perspectiveMatrix;

out vec3 FragPos;

void main()
{
    gl_Position = perspectiveMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);

    FragPos = vec3(modelMatrix * vec4(aPos, 1.0));
}

@end

@fs
#version 460 core

out vec4 FragColor;

in vec3 FragPos;

void main(){
    // checker pattern
    int x = int(FragPos.x) % 2;
    int y = int(FragPos.y) % 2;
    int z = int(FragPos.z) % 2;

    vec3 color = vec3(1.0);

    if (abs(x) + abs(y) + abs(z) == 1){
        color *= 0.7f;
    }
    else {
        color *= 0.3f;
    }

    FragColor = vec4(color, 1.0);
}

@end