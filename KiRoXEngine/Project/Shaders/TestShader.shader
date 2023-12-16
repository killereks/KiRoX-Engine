@vs
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 perspectiveMatrix;

out DATA {
	vec3 FragPos;
	vec3 Vertex;
	vec3 Normal;
	vec2 UV;
	vec3 ClipSpacePos;
} vs_out;

void main(){
	gl_Position = perspectiveMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);

	vs_out.FragPos = vec3(modelMatrix * vec4(aPos, 1.0));
	vs_out.Vertex = aPos;
	vs_out.Normal = aNormal;
	vs_out.UV = aUV;
	vs_out.ClipSpacePos = gl_Position.xyz;
}

@end


@gs
#version 460 core

uniform vec3 boundingBoxMin;
uniform vec3 boundingBoxMax;

// frustum culling
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in DATA {
	vec3 FragPos;
	vec3 Vertex;
	vec3 Normal;
	vec2 UV;
	vec3 ClipSpacePos;
} gs_in[];

out vec3 FragPos;
out vec3 Vertex;
out vec3 Normal;
out vec2 UV;

uniform mat4 viewMatrix;
uniform mat4 perspectiveMatrix;
uniform mat4 modelMatrix;

bool IsInFrustum(){
	// transform the bounding box from world space to clip space
	vec4 clipSpaceMin = perspectiveMatrix * viewMatrix * vec4(boundingBoxMin, 1.0);
	vec4 clipSpaceMax = perspectiveMatrix * viewMatrix * vec4(boundingBoxMax, 1.0);

	clipSpaceMin /= clipSpaceMin.w;
	clipSpaceMax /= clipSpaceMax.w;
	
	// if either the min or max is inside the frustum, we draw
	bool isMinInside = all(lessThan(clipSpaceMin.xyz, vec3(1.0))) && all(greaterThan(clipSpaceMin.xyz, vec3(-1.0)));
	bool isMaxInside = all(lessThan(clipSpaceMax.xyz, vec3(1.0))) && all(greaterThan(clipSpaceMax.xyz, vec3(-1.0)));

	return isMinInside || isMaxInside;
}

void main(){
	if (IsInFrustum()){
		gl_Position = gl_in[0].gl_Position;
		FragPos = gs_in[0].FragPos;
		Vertex = gs_in[0].Vertex;
		Normal = gs_in[0].Normal;
		UV = gs_in[0].UV;
		EmitVertex();

		gl_Position = gl_in[1].gl_Position;
		FragPos = gs_in[1].FragPos;
		Vertex = gs_in[1].Vertex;
		Normal = gs_in[1].Normal;
		UV = gs_in[1].UV;
		EmitVertex();

		gl_Position = gl_in[2].gl_Position;
		FragPos = gs_in[2].FragPos;
		Vertex = gs_in[2].Vertex;
		Normal = gs_in[2].Normal;
		UV = gs_in[2].UV;
		EmitVertex();

		EndPrimitive();
	}
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
	vec3 lightPos = vec3(0, 20, 0);
	vec3 lightDir = normalize(lightPos - FragPos);

	float light = max(0.0, dot(lightDir, Normal));

	vec3 col = texture(albedo, UV).rgb * light;
	col = light * vec3(1.0, 0.0, 0.0);

	FragColor = vec4(col, 1.0);
}

@end