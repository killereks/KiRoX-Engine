@vs
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 perspectiveMatrix;

uniform mat4 lightSpaceMatrix;

out vec3 FragPos;
out vec3 Vertex;
out vec3 Normal;
out vec2 UV;

out vec4 FragPosLightSpace;

void main(){
	gl_Position = perspectiveMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);

	FragPos = vec3(modelMatrix * vec4(aPos, 1.0));
	Vertex = aPos;
	Normal = normalize(transpose(inverse(mat3(modelMatrix))) * aNormal);
	UV = aUV;

	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}

@end

@fs
#version 460 core

in vec3 FragPos;
in vec3 Vertex;
in vec3 Normal;
in vec2 UV;

in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform sampler2D shadowMap;

float num(vec4 seed){
	float dot_product = dot(seed, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}

float shadowCalc(float dotLightNormal, vec2 offset){
	float bias = max(0.0005 * (1.0 - dotLightNormal), 0.0005);
	vec3 pos = FragPosLightSpace.xyz / FragPosLightSpace.w;
	pos = pos * 0.5 + 0.5;

	if (pos.z > 1.0) return 0.0;

	float rand = num(vec4(pos.xy, pos.y, pos.x)) * 0.001;
	float depth = texture(shadowMap, pos.xy + offset + rand).r;

	if (depth + bias < pos.z) return 0.0;
	return 1.0;
}

float softShadows(float dotLightNormal){
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	float shadow = 0.0;

	int size = 2;
	for(int x = -size; x <= size; ++x){
		for(int y = -size; y <= size; ++y){
			vec2 sampleOffset = vec2(x,y) * texelSize;
			shadow += shadowCalc(dotLightNormal, sampleOffset);
		}
	}

	shadow /= (2 * size + 1) * (2 * size + 1);
	return shadow;
}

void main(){
	vec3 lightPos = vec3(0, 10, 0);
	vec3 lightDir = normalize(lightPos - FragPos);

	float light = max(0.0, dot(lightDir, Normal));
	
	// checkerboard
	bool isEven = (int(round(FragPos.x)) + int(round(FragPos.z)) + int(round(FragPos.y))) % 2 == 0;
	vec3 col = vec3(0.5);
	if (isEven){
		col = vec3(0.2);
	}

	float shadow = softShadows(dot(lightDir, Normal));

	col *= light * shadow;

	FragColor = vec4(col, 1.0);
}

@end