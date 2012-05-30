#version 150 core

struct Light{
	vec3 position;
};

uniform Light light;
uniform sampler2D inSampler;

smooth in vec3 pass_Normal;
smooth in vec3 pass_Position;
smooth in vec2 TexCoord;

out vec4 out_Color;

void main(void){
	vec3 Normal = normalize(pass_Normal);
	vec3 light_Direction = normalize(light.position-pass_Position);
	vec3 camera_Direction = normalize(-pass_Position);
	vec3 half_vector = normalize(camera_Direction + light_Direction);
	
	float diffuse = max(0.2, dot(Normal, light_Direction));
	vec3 temp_Color = diffuse * vec3(1.0);
	
	float specular = max( 0.0, dot( Normal, half_vector) );
	
	if(diffuse != 0){
		float fspecular = pow(specular, 128.0);
		temp_Color += fspecular;
	}
	// out_Color = vec4(temp_Color,1.0);
	out_Color = texture(inSampler, TexCoord) * vec4(temp_Color,1.0);
}