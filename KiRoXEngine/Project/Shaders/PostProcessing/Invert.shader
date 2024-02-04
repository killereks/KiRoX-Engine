@vs
#version 460 core

layout (location = 0) in vec2 inPos;

out vec2 texCoords;

void main(){
    gl_Position = vec4(inPos.x, inPos.y, 0.0, 1.0);
    texCoords = inPos.xy * 0.5 + 0.5;
}
@end
@fs
#version 460 core

in vec2 texCoords;

uniform sampler2D screenTexture;
uniform sampler2D shadowMap;
uniform sampler2D cameraDepthMap;

// CAMERA
uniform vec3 camPos;
uniform vec3 camFwd;
uniform vec3 camUp;
uniform vec3 camRight;

uniform float nearPlane;
uniform float farPlane;

uniform mat4 projection;
uniform mat4 view;

uniform vec3 lightDir;
uniform mat4 lightSpaceMatrix;

uniform float time;

out vec4 FragColor;

#define VL_STEP_COUNT 100.0
#define VL_ATTENUATION_FACTOR 0.1

#define SATURATION 0.5

float lineariseDepth(float depth){
    float near = nearPlane;
    float far = farPlane;
    return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

float random(vec2 st){
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453);
}

vec3 getWorldPosition(vec2 TexCoord){
    float depth = texture(cameraDepthMap, TexCoord).r;

    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(TexCoord * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = inverse(projection) * clipSpacePosition;

    viewSpacePosition /= viewSpacePosition.w;

    vec4 worldSpacePosition = inverse(view) * viewSpacePosition;

    return worldSpacePosition.xyz;
}

float worldToShadow(vec3 worldPos){
    // returns if world position is in shadow
    vec4 lightSpacePos = lightSpaceMatrix * vec4(worldPos, 1.0);
    lightSpacePos /= lightSpacePos.w;

    lightSpacePos = lightSpacePos * 0.5 + 0.5;

    float shadow = 1.0;

    float bias = 0.005;

    float closestDepth = texture(shadowMap, lightSpacePos.xy).r;
    float currentDepth = lightSpacePos.z;

    if(currentDepth - bias > closestDepth){
        shadow = 0.0;
    }

    return shadow;
}

#define G_SCATTERING 0.76
#define PI 3.141592653589793

// Mie scaterring, approximated with Henyey-Greenstein phase function
float ComputeScattering(float lightDotView){
    float result = 1.0 - G_SCATTERING * G_SCATTERING;
    result /= (4.0 * PI * pow(1.0 + G_SCATTERING * G_SCATTERING - (2.0 * G_SCATTERING) * lightDotView, 1.5));
    return result;
}

// UV = [0,1]
// screenPos = [-1,1]
vec3 VolumetricLighting(vec3 color, vec2 uv, vec2 screenPos){    
    float lum = 0.0;

    float camDepth = lineariseDepth(texture(cameraDepthMap, uv).r);

    vec3 worldPos = getWorldPosition(uv);
    
    vec3 rayDir = normalize(worldPos - camPos);
    float vl_distance = length(worldPos - camPos);

    float stepSize = 1.0 / VL_STEP_COUNT;
    float sampleDepth = random(uv + time * 0.01) * stepSize * vl_distance;

    for (int i = 0; i < VL_STEP_COUNT; i++){
        vec3 pos = camPos + rayDir * sampleDepth;

        float shadow = worldToShadow(pos);

        if (sampleDepth >= camDepth) break;

        float attenuation = exp(-VL_ATTENUATION_FACTOR * sampleDepth);
        lum += ComputeScattering(dot(rayDir, lightDir)) * shadow * attenuation;

        float rand = random(uv + time * 0.01) * 0.1 + 0.95;

        sampleDepth += stepSize * vl_distance * rand;
    }

    // clamp lum
    lum = clamp(lum, 0.0, 0.2);
    
    return color + lum;
}

vec3 Vignette(vec3 colorInput, vec2 uv){
    float vignette = 3.0;
    float vignetteRadius = 0.5;
    float vignetteSoftness = 0.6;

    vec2 vignetteCenter = vec2(0.5, 0.5);
    vec2 vignetteDelta = uv - vignetteCenter;
    float vignetteDistance = length(vignetteDelta);
    float vignetteIntensity = smoothstep(vignetteRadius, vignetteRadius + vignetteSoftness, vignetteDistance);
    vignetteIntensity = 1.0 - vignetteIntensity * vignette;

    return colorInput * vignetteIntensity;
}

vec3 ACES(vec3 colorInput){
    mat3x3 rgb2aces = mat3x3(
        1.0498110175, 0.0000000000, -0.0000974845,
        -0.4959030231, 1.3733130458, 0.0982400361,
        0.0000000000, 0.0000000000, 0.9912520182
    );

    return colorInput * rgb2aces;
}

vec3 Saturation(vec3 color){
    float luma = dot(color, vec3(0.2126729, 0.7151522, 0.0721750));
    return vec3(luma) + vec3(SATURATION) * (color - vec3(luma));
}

void main(){
    vec3 col = texture(screenTexture, texCoords).rgb;

    col = VolumetricLighting(col, texCoords, texCoords * 2.0 - 1.0);

    //col = Saturation(col);
    //col = ACES(col);
    //col = Vignette(col, texCoords);
    
    //col = getWorldPosition(texCoords);

    FragColor = vec4(col, 1.0);
}


@end