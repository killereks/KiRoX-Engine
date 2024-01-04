@vs
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 perspectiveMatrix;

uniform mat4 lightSpaceMatrix;

out vec3 FragPos;
out vec3 Vertex;
out vec3 Normal;
out vec2 UV;

out mat3 TBN;

out vec4 FragPosLightSpace;

void main(){
	gl_Position = perspectiveMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);

	FragPos = vec3(modelMatrix * vec4(aPos, 1.0));
	Vertex = aPos;
	Normal = normalize(transpose(inverse(mat3(modelMatrix))) * aNormal);
	UV = aUV;

	TBN = mat3(normalize(vec3(modelMatrix * vec4(aTangent, 0.0))),
			   normalize(vec3(modelMatrix * vec4(aBitangent, 0.0))),
			   normalize(vec3(modelMatrix * vec4(aNormal, 0.0))));

	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}

@end

@fs
#version 460 core

in vec3 FragPos;
in vec3 Vertex;
in vec3 Normal;
in vec2 UV;

in mat3 TBN;

// light stuff
uniform vec3 lightDir;

// camera stuff
uniform vec3 viewPos;
uniform vec3 viewForward;

in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform bool hasNormalMap;

uniform sampler2D shadowMap;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;

float num(vec4 seed){
	float dot_product = dot(seed, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}

float shadowCalc(float dotLightNormal, vec2 offset){
	float bias = max(0.0005 * (1.0 - dotLightNormal), 0.0005);
	vec3 pos = FragPosLightSpace.xyz / FragPosLightSpace.w;
	pos = pos * 0.5 + 0.5;

	if (pos.z > 1.0) return 0.0;

	float rand = num(vec4(pos.xy, pos.y, pos.x)) * 0.0005;
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
	vec3 normals = Normal;
	if (hasNormalMap){
		normals = normalize(TBN * normalize(texture(normalMap, UV).rgb * 2.0 - 1.0));
	}

	vec3 albedo = texture(albedoMap, UV).rgb;
	float metallic = texture(metallicMap, UV).r;

	float shadow = softShadows(dot(lightDir, normals));

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectedDir = reflect(lightDir, normals);
	float specular = pow(max(dot(viewDir, reflectedDir), 0.0), 32) * metallic.r;

	float diffuse = max(dot(normals, -lightDir), 0.0);

	//vec3 col = albedo * (diffuse + specular) * shadow;
	vec3 col = albedo * (metallic * diffuse + metallic * specular) * shadow;

	FragColor = vec4(col, 1.0);
}

@end