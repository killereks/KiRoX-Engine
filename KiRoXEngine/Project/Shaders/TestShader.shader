@vs
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 perspectiveMatrix;

out vec3 FragPos;
out vec3 Vertex;
out vec3 Normal;
out vec2 UV;

void main(){
	gl_Position = perspectiveMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);

	FragPos = vec3(modelMatrix * vec4(aPos, 1.0));
	Vertex = aPos;
	Normal = aNormal;
	UV = aUV;
}

@end

@fs
#version 460 core

in vec3 FragPos;
in vec3 Vertex;
in vec3 Normal;
in vec2 UV;

uniform sampler2D albedo;

out vec4 FragColor;

void main(){
	vec3 lightPos = vec3(0, 10, 0);
	vec3 lightDir = normalize(lightPos - FragPos);

	float light = max(0.0, dot(lightDir, Normal));

	vec3 col = texture(albedo, UV).rgb * light;

	FragColor = vec4(col, 1.0);
}

@end