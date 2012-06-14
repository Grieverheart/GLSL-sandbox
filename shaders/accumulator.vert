#version 330 core

uniform float scale;

layout(location = 0) in vec3 in_Position;
layout(location = 2) in vec2 in_TexCoord;

smooth out vec2 pass_TexCoord;

void main(void){
	pass_TexCoord = in_TexCoord;
	gl_Position = mat4(1.0) * vec4(scale * in_Position, 1.0);
}