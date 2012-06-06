#include "../include/g-buffer.h"
#include <iomanip>

CGBuffer::CGBuffer(void){
    m_fbo = 0;
}

CGBuffer::~CGBuffer(void){
	GLuint array_size = sizeof(m_textures)/sizeof(m_textures[0]);
	if(m_textures != NULL){
		glDeleteTextures(array_size, &m_textures);
	}
	if(m_depthTexture != NULL){
		glDeleteTextures(1, &m_depthTexture);
	}
    if(m_fbo != NULL){
		glDeleteFramebuffers(1, &m_fbo);
    }
}

bool CGBuffer::Init(unsigned int WindowWidth, unsigned int WindowHeight){
	//Create FBO
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	
	GLuint array_size = sizeof(m_textures)/sizeof(m_textures[0]);
	
	//Create gbuffer and Depth Buffer Textures
	glGenTextures(array_size, &m_textures);
	glGenTextures(1, &m_fbo);
	
	//prepare gbuffer
	for(unsigned int i = 0; i < arra_size; i++){
		glBindTexture(GL_TEXTURE_2D, m_textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i], 0);
	}
	//prepare depth buffer
	
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMap, 0);
	
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	// glm::vec4 borderColor = glm::vec4(1.0,glm::vec3(0.0));
	// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &borderColor[0]);
	
	GLenum DrawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
	glDrawBuffers(array_size, DrawBuffers);
	
	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	
	if(Status != GL_FRAMEBUFFER_COMPLETE){
		std::cout << "FB error, status 0x" << std::hex << Status << std::endl;
		return false;
	}
	
	//Restore default framebuffer
	glBindBuffer(GL_DRAW_FRAMEBUFFER, 0);
	
	return true;
}

void CGBuffer::BindForWriting(void){
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}

void CGBuffer::BindForReading(GLenum TextureUnit){
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
}

void CGBuffer::SetReadBuffer(GBUFF_TEXTURE_TYPE TextureType){
	glReadBuffer(GL_COLOR_ATTACHMENT0 + TextureType);
}