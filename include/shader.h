#ifndef __SHADER_H
#define __SHADER_H

#include "main.h"

class Shader{
public:
	Shader(void);
	Shader(const char *vsFile, const char *fsFile);
	~Shader(void);
	
	void init(const char *vsFile, const char *fsFile);
	
	void bind(void);
	void unbind(void);
	GLuint id(void);
	
private:
	GLuint shader_id;
	GLuint shader_vp;
	GLuint shader_fp;
	
	bool initialized;
};

#endif