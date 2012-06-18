#include "../include/ssao.h"
#include <cstdlib>
#include <time.h>

static inline float random(float x, float y){
	float one = (float)rand() / RAND_MAX;
	float dx = y - x;
	return dx * one + x;
}

static inline float lerp(float start, float end, float perc){
	return start + perc * (end - start);
}

Cssao::Cssao(void){
	srand(time(NULL));
	m_kernel_size = 16;
	m_noise_size = 6;
	m_RADIUS = 1.2f;
}

Cssao::~Cssao(void){
	delete[] m_kernel;
	delete[] m_noise;
}

void Cssao::CreateKernel(void){	
	m_kernel = new glm::vec3[m_kernel_size];
	
	for(unsigned int i = 0; i < m_kernel_size; i++){
		m_kernel[i] = glm::vec3(
			random(-1.0f, 1.0f),
			random(-1.0f, 1.0f),
			random(0.0f, 1.0f)
		);
		float scale = (float)i / (float)m_kernel_size;
		scale = lerp(0.1f, 1.0f, scale * scale);
		m_kernel[i] *= scale;
	}
}

void Cssao::CreateNoise(void){
	m_noise = new glm::vec3[m_noise_size * m_noise_size];
	
	for(unsigned int i = 0; i < m_noise_size * m_noise_size; i++){
		m_noise[i] = glm::vec3(
			random(-1.0f, 1.0f),
			random(-1.0f, 1.0f),
			0.0f
		);
		m_noise[i] = glm::normalize(m_noise[i]);
	}
	
	glGenTextures(1, &m_noise_texture);
	glBindTexture(GL_TEXTURE_2D, m_noise_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, m_noise_size, m_noise_size, 0, GL_RGB, GL_FLOAT, &m_noise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

bool Cssao::Init(unsigned int WindowWidth, unsigned int WindowHeight, unsigned int shader_id){
	m_shader_id = shader_id;
	
	m_nScale_kSize = glm::ivec3(WindowWidth / m_noise_size, WindowHeight / m_noise_size, m_kernel_size);
	
	CreateKernel();
	CreateNoise();
	
	m_kernLocation = glGetUniformLocation(m_shader_id, "kernel");
	m_noiseSamplerLocation = glGetUniformLocation(m_shader_id, "noise");
	m_nScale_kSizeLocation = glGetUniformLocation(m_shader_id, "noiseScale_kernelSize");
	m_RADIUSLocation = glGetUniformLocation(m_shader_id, "RADIUS");
	
	if(
		m_kernLocation == -1 || m_nScale_kSizeLocation == -1 || 
		m_noiseSamplerLocation == -1 || m_RADIUSLocation == -1
	){
		std::cout << "Couldn't bind SSAO uniforms" << std::endl;
	}
	return true;
}

void Cssao::UploadUniforms(void){
	glUniform1i(m_noiseSamplerLocation, 2); // Set to apropriate texture unit
	glUniform3iv(m_nScale_kSizeLocation, 1, &m_nScale_kSize[0]);
	glUniform3fv(m_kernLocation, m_kernel_size, &m_kernel[0][0]);
	glUniform1fv(m_RADIUSLocation, 1, &m_RADIUS);
}

void Cssao::BindNoise(void){
	glActiveTexture(GL_TEXTURE0 + 2); // Set to apropriate texture unit
	glBindTexture(GL_TEXTURE_2D, m_noise_texture);
}