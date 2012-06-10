#version 330 core

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVPMatrix;
// uniform mat4 lightMVP;

uniform float scale;

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_TexCoord;

smooth out vec3 pass_Normal;
smooth out vec3 pass_Position;
smooth out vec2 TexCoord;
// smooth out vec4 LSTexCoord;

const mat4 TexMatrix = mat4(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
);

void main(void){

	pass_Position = (ModelViewMatrix * vec4(scale * in_Position, 1.0)).xyz;
	pass_Normal = NormalMatrix * in_Normal; 
	TexCoord = in_TexCoord;
	// LSTexCoord = TexMatrix * lightMVP * vec4(scale * in_Position, 1.0);
	
	gl_Position = MVPMatrix * vec4(scale * in_Position, 1.0);
}