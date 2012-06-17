#version 330 core

uniform sampler2D inSampler;

smooth in vec3 pass_Normal;
smooth in vec2 TexCoord;

layout(location = 0) out vec3 outDiffuse;
layout(location = 1) out vec3 outNormal;

void main(void){
	outDiffuse = texture(inSampler, TexCoord).xyz;
	outNormal = pass_Normal;
}