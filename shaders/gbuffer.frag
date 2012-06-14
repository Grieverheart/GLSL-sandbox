#version 330 core

uniform sampler2D inSampler;

smooth in vec3 pass_Normal;
smooth in vec3 pass_Position;
smooth in vec2 TexCoord;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outDiffuse;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec3 outTexCoord;

void main(void){
	outPosition = pass_Position;
	outDiffuse = texture(inSampler, TexCoord).xyz;
	outNormal = pass_Normal;
	outTexCoord = vec3(TexCoord, 0.0);
}