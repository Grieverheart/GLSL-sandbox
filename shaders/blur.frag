#version 330 core

uniform sampler2D aoSampler;
uniform vec2 TEXEL_SIZE; // x = 1/res x, y = 1/res y

noperspective in vec2 TexCoord;

out vec4 out_AO;

void main(void){
	float result = 0.0;
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			vec2 offset = vec2(TEXEL_SIZE.x * float(j), TEXEL_SIZE.y * float(i));
			result += texture(aoSampler, TexCoord + offset).r;
		}
	}
	
	out_AO = vec4(vec3(0.0), result * 0.0625);
}