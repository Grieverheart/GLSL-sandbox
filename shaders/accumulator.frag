#version 330 core

struct Light{
	vec3 direction;
};

uniform ivec2 ScreenSize;
uniform Light light;

uniform sampler2D PositionMap;
uniform sampler2D ColorMap;
uniform sampler2D NormalMap;

out vec4 out_Color;

vec2 CalcTexCoord(void){
	return gl_FragCoord.xy / ScreenSize;
}

vec4 CalcLight(vec3 position, vec3 normal){
	
	vec4 DiffuseColor = vec4(0.0);
	vec4 SpecularColor = vec4(0.0);
	
	vec3 light_Direction = -normalize(light.direction);
	float diffuse = max(0.0, dot(normal, light_Direction));
	
	if(diffuse > 0.0){
		DiffuseColor = diffuse * vec4(1.0);
		
		vec3 camera_Direction = normalize(-position);
		vec3 half_vector = normalize(camera_Direction + light_Direction);
		
		float specular = max(0.0, dot(normal, half_vector));
		float fspecular = pow(specular, 128.0);
		SpecularColor = fspecular * vec4(1.0);
	}
	return DiffuseColor + SpecularColor + vec4(0.1);
}

void main(void){

	vec2 TexCoord = CalcTexCoord();
	vec3 Position = texture(PositionMap, TexCoord).xyz;
	vec3 Color = texture(ColorMap, TexCoord).xyz;
	vec3 Normal = normalize(texture(NormalMap, TexCoord).xyz);
	
	out_Color = vec4(Color, 1.0) * CalcLight(Position, Normal);
}