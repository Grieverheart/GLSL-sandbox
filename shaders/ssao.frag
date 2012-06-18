#version 330 core

uniform sampler2D DepthMap;
uniform sampler2D noise;
uniform sampler2D NormalMap;

uniform vec2 projAB;
uniform ivec3 noiseScale_kernelSize;
uniform vec3 kernel[16];
uniform float RADIUS;
uniform mat4 projectionMatrix;

smooth in vec2 pass_TexCoord;
smooth in vec3 viewRay;

out vec4 out_Color;

vec3 CalcPosition(void){
	float depth = texture(DepthMap, pass_TexCoord).r;
	float linearDepth = projAB.y / (depth - projAB.x);
	return linearDepth * viewRay;
}

mat3 CalcRMatrix(vec3 normal, vec2 texcoord){
	ivec2 noiseScale = noiseScale_kernelSize.xy;
	vec3 rvec = texture(noise, texcoord * noiseScale).xyz;
	vec3 tangent = normalize(rvec - normal * dot(rvec, normal));
	vec3 bitangent = cross(normal, tangent);
	
	return mat3(tangent, bitangent, normal);
}

void main(void){

	vec2 TexCoord = pass_TexCoord;
	vec3 Position = CalcPosition();
	vec3 Normal = normalize(texture(NormalMap, TexCoord).xyz);
	
	mat3 RotationMatrix = CalcRMatrix(Normal, TexCoord);
	
	int kernelSize = noiseScale_kernelSize.z;
	
	float occlusion = 0.0;
	
	for(int i = 0; i < kernelSize; i++){
		// Get sample position
		vec3 sample = RotationMatrix * kernel[i];
		sample = sample * RADIUS + Position;
		// Project and bias sample position to get its texture coordinates
		vec4 offset = projectionMatrix * vec4(sample, 1.0);
		offset.xy /= offset.w;
		offset.xy = offset.xy * 0.5 + 0.5;
		// Get sample depth
		float sample_depth = texture(DepthMap, offset.xy).r;
		if(abs(Position.z - sample_depth) < RADIUS){
			occlusion += (sample_depth <= sample.z ? 1.0 : 0.0);
		}
		occlusion = 1.0 - (occlusion / kernelSize);
	}
	
	out_Color = vec4(vec3(occlusion), 1.0);
}