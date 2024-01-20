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

#define PI 3.14159265359

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
uniform bool hasMetallicMap;
uniform bool hasHeightMap;

uniform vec2 tiling;
uniform float roughness;

uniform sampler2D shadowMap;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D heightMap;

uniform samplerCube skybox;

//uniform sampler2DArray shadowMapArray;
//uniform int numLights;
//uniform mat4 lightSpaceMatrices[16];

//struct SpotLightData {
//	vec3 pos;
//	vec3 dir;
//	float angle;
//};
//uniform SpotLightData spotLightData[16];

//struct ShadowParameters {
//	mat4 lightSpaceMatrix;
//	vec4 fragPosLightSpace;
//	float bias;
//};

float randomNum(vec4 seed){
	float dot_product = dot(seed, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}

vec3 RandomVector(vec3 seed){
	return vec3(randomNum(vec4(seed, 0.0)),
				randomNum(vec4(seed, 1.0)),
				randomNum(vec4(seed, 2.0)));
}

vec3 RandomVectorOnHemisphere(vec3 seed, vec3 forward){
	vec3 rand = RandomVector(seed);
	if (dot(rand, forward) < 0.0){
		rand *= -1.0;
	}
	
	return rand;
}

float shadowCalcDirectional(float dotLightNormal, vec2 offset){
	float bias = max(0.0005 * (1.0 - dotLightNormal), 0.0005);
	vec3 pos = FragPosLightSpace.xyz / FragPosLightSpace.w;
	pos = pos * 0.5 + 0.5;

	if (pos.z > 1.0) return 0.0;

	float rand = randomNum(vec4(pos.xy, pos.y, pos.x)) * 0.0005;
	float depth = texture(shadowMap, pos.xy + offset + rand).r;

	if (depth + bias < pos.z) return 0.0;
	return 1.0;
}

float softShadowsDirectional(float dotLightNormal){
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	float shadow = 0.0;

	int size = 2;
	for(int x = -size; x <= size; ++x){
		for(int y = -size; y <= size; ++y){
			vec2 sampleOffset = vec2(x,y) * texelSize;
			shadow += shadowCalcDirectional(dotLightNormal, sampleOffset);
		}
	}

	shadow /= (2 * size + 1) * (2 * size + 1);
	return shadow;
}

/// PBR
float DistributionGGX(vec3 N, vec3 H, float roughness){
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness){
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness){
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0){
    return F0 + (1.0 - F0)*pow((1.0 + 0.000001) - cosTheta, 5.0);
}

vec3 CalcPBRLighting(vec3 normal){
	float roughness = 0.0;
	if (hasMetallicMap){
		roughness = texture(metallicMap, UV).g;
	}

	vec3 F0 = vec3(0.04);

	vec3 albedo = texture(albedoMap, UV * tiling).rgb;

	// Cook-Torrance microfacet specular reflection
    vec3 halfDir = normalize(viewForward + -lightDir);
    float D = DistributionGGX(normal, halfDir, roughness);
    float G = GeometrySmith(normal, viewForward, -lightDir, roughness);
    vec3 specular = vec3(1.0) * D * G / (4.0 * max(dot(normal, viewForward), 0.001) * max(dot(normal, -lightDir), 0.001));

    // Fresnel-Schlick approximation for the reflection
    float fresnel = fresnelSchlick(max(dot(halfDir, viewForward), 0.0), F0).x;

    // Lambertian diffuse reflection
    float kS = fresnel;
    float kD = 1.0 - kS;
    vec3 diffuse = (albedo / PI) * (1.0 - kS);

	float lightIntensity = 3.0;

    // Final PBR lighting equation
    vec3 lighting = lightIntensity * (kD * diffuse + specular) * max(dot(normal, -lightDir), 0.0);

    return lighting;
}

void main(){
	vec3 normals = Normal;
	vec2 UVs = UV;
	
	UVs.x *= tiling.x;
	UVs.y *= tiling.y;

	// HEIGHT MAP
	if (hasHeightMap){
		vec3 viewDir = normalize(viewPos - FragPos);
		viewDir *= TBN;

		float heightScale = 0.005;
		const float minLayers = 8.0;
		const float maxLayers = 64.0;

		float numLayers = mix(minLayers, maxLayers, abs(viewForward.z));
		float layerDepth = 1.0 / numLayers;
		
		float currentLayerDepth = 0.0;
		vec2 S = viewDir.xy / viewDir.z * heightScale;

		vec2 deltaUVs = S / numLayers;
		float currentDepthMapValue = 1.0 - texture(heightMap, UVs).r;

		while (currentLayerDepth < currentDepthMapValue){
			UVs -= deltaUVs;
			currentDepthMapValue = 1.0 - texture(heightMap, UVs).r;
			currentLayerDepth += layerDepth;
		}

		vec2 prevTexCoords = UVs + deltaUVs;
		float afterDepth = currentDepthMapValue - currentLayerDepth;
		float beforeDepth = 1.0 - texture(heightMap, prevTexCoords).r + layerDepth - currentLayerDepth;
		float weight = afterDepth / (afterDepth - beforeDepth);
		UVs = prevTexCoords * weight + UVs * (1.0 - weight);

		if (UVs.x < 0.0 || UVs.x > 1.0 || UVs.y < 0.0 || UVs.y > 1.0){
			discard;
		}
	}

	// END HEIGTH MAP

	if (hasNormalMap){
		normals = normalize(TBN * normalize(texture(normalMap, UVs).rgb * 2.0 - 1.0));
	}

	float metallic = 0.0;
	if (hasMetallicMap){
		metallic = texture(metallicMap, UVs).r;
	}

	vec3 albedo = texture(albedoMap, UVs).rgb;

	///////////////////////////////////////////
	// SPOT LIGHT SHADOWS
	vec3 col = vec3(0.0);

	//for (int i = 0; i < numLights; i++){
	//	SpotLightData lightData = spotLightData[i];

	//	ShadowParameters shadowParams;

	//	shadowParams.lightSpaceMatrix = lightSpaceMatrices[i];
	//	shadowParams.fragPosLightSpace = lightSpaceMatrices[i] * vec4(FragPos, 1.0);
	//	shadowParams.bias = 0.00005;

	//	float shadow = softSpotLightShadows(shadowParams, shadowMapArray, i);

	//	vec3 viewDir = normalize(viewPos - FragPos);
	//	vec3 reflectedDir = reflect(lightDir, normals);
	//	float specular = pow(max(dot(viewDir, reflectedDir), 0.0), 32) * metallic.r;

	//	//float diffuse = max(dot(normals, lightDir), 0.0);

	//	col += albedo * (metallic * diffuse + metallic * specular) * shadow;
	//}

	///////////////////////////////////////////
	// DIRECTIONAL LIGHT SHADOWS

	float shadow = softShadowsDirectional(dot(lightDir, normals));

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectedDir = reflect(lightDir, normals);
	float specular = pow(max(dot(viewDir, reflectedDir), 0.0), 32) * metallic.r;

	float diffuse = max(dot(normals, -lightDir), 0.0);

	///////////////////////////////////////////
	// PBR

	//col += albedo * (CalcPBRLighting(normals) + specular) * shadow;

	//col += CalcPBRLighting(normals) * shadow;

	col += CalcPBRLighting(normals) * shadow;

	// calculate skybox reflection
	vec3 I = normalize(FragPos - viewPos);
	vec3 R = reflect(I, normals);
	R = mix(R, RandomVectorOnHemisphere(R, normals), roughness);
	vec3 skyboxCol = texture(skybox, R).rgb;
	col = mix(col, skyboxCol * shadow, metallic);

	//col += albedo * (metallic * diffuse + metallic * specular) * shadow;

	FragColor = vec4(col, 1.0);
}

@end