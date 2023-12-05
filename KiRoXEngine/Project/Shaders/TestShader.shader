@vs
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 perspectiveMatrix;

out vec3 Vertex;
out vec3 Normal;
out vec2 UV;

void main(){
    gl_Position = perspectiveMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);

    Vertex = aPos;
	Normal = aNormal;
	UV = aUV;
}

@end

@fs
#version 460 core

in vec3 Vertex;
in vec3 Normal;
in vec2 UV;

uniform sampler2D albedo;

out vec4 FragColor;

void main(){
    float tiling = 30.0;

    float x = floor(Vertex.x * tiling);
    float y = floor(Vertex.y * tiling);
    float result = sign(mod(x + y, 2.0));

    if (result < 1.0) {
        FragColor = vec4(0.5, 0.5, 0.5, 1.0);
    } else {
        FragColor = vec4(0.8, 0.8, 0.8, 1.0);
    }
	
	vec3 col = texture(albedo, UV).rgb;
	
	FragColor = vec4(col, 1.0);
}
@end