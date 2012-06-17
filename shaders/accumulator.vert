#version 330 core

uniform mat4 invProjMatrix;

layout(location = 0) in vec3 in_Position;
layout(location = 2) in vec2 in_TexCoord;

smooth out vec2 pass_TexCoord;
smooth out vec3 viewRay;

void main(void){
	pass_TexCoord = in_TexCoord;
	viewRay = (invProjMatrix * vec4(in_Position, 1.0)).xyz;
	viewRay = vec3(viewRay.xy / viewRay.z, 1.0);
	gl_Position = mat4(1.0) * vec4(in_Position, 1.0);
}