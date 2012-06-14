#include "../include/g-buffer.h"
#include <iomanip>

CGBuffer::CGBuffer(void){
    m_fbo = 0;
	m_depthTexture = 0;
}

CGBuffer::~CGBuffer(void){
	GLuint array_size = sizeof(m_textures)/sizeof(m_textures[0]);
	if(m_textures != NULL){
		glDeleteTextures(array_size, &m_textures[0]);
	}
	if(m_depthTexture != 0){
		glDeleteTextures(1, &m_depthTexture);
	}
    if(m_fbo != 0){
		glDeleteFramebuffers(1, &m_fbo);
    }
}

bool CGBuffer::Init(unsigned int WindowWidth, unsigned int WindowHeight){
	//Create FBO
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	
	GLuint array_size = sizeof(m_textures)/sizeof(m_textures[0]);
	
	//Create gbuffer and Depth Buffer Textures
	glGenTextures(array_size, &m_textures[0]);
	glGenTextures(1, &m_depthTexture);
	
	//prepare gbuffer
	for(unsigned int i = 0; i < array_size; i++){
		glBindTexture(GL_TEXTURE_2D, m_textures[i]);
		if(i == 0) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
		else if(i == 2) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
		else glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i], 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	//prepare depth buffer
	
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);
	
	GLenum DrawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
	glDrawBuffers(array_size, DrawBuffers);
	
	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	
	if(Status != GL_FRAMEBUFFER_COMPLETE){
		std::cout << "FB error, status 0x" << std::hex << Status << std::endl;
		return false;
	}
	
	//Restore default framebuffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	
	return true;
}

void CGBuffer::BindForWriting(void){
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}

void CGBuffer::BindForReading(void){
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	for(unsigned int i = 0; i < GBUFF_NUM_TEXTURES; i++){
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textures[GBUFF_TEXTURE_TYPE_POSITION + i]);
	}
}

void CGBuffer::SetReadBuffer(GBUFF_TEXTURE_TYPE TextureType){
	glReadBuffer(GL_COLOR_ATTACHMENT0 + TextureType);
}