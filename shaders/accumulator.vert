#version 330 core

uniform float scale;

layout(location = 0) in vec3 in_Position;

void main(void){
	gl_Position = mat4(1.0) * vec4(scale * in_Position, 1.0);
}