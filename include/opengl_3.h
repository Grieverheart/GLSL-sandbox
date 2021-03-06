#ifndef __OPENGL_3_H
#define __OPENGL_3_H

#include <string>
#include <vector>
#include "main.h"
#include "shader.h"
#include "obj_parser.h"
#include "mesh.h"
#include "camera.h"
#include "texture.h"
#include "shadowmap_FBO.h"
#include "light.h"

class OpenGLContext{
public:
	OpenGLContext(void); 				// Default constructor  
	~OpenGLContext(void); 				// Destructor for cleaning up our application  
	bool create30Context(void); 	// Creation of our OpenGL 3.x context  
	void setupScene(int argc, char *argv[]); 				// All scene information can be setup here  
	void reshapeWindow(int w, int h); 	// Method to get our window width and height on resize  
	void renderScene(void); 			// Render scene (display method from previous OpenGL tutorials)
	void processScene(void);
	void fboPass(void);
	void drawPass(void);
	
	void toggleState(std::string state);
	bool getState(std::string state);
	float getZoom(void);
	void setZoom(float zoom);
	glm::ivec2 getScreen(void);
	
	void multMatrix(std::string matrix1, glm::mat4 matrix2);
	void setMatrix(std::string matrix1, glm::mat4 matrix2);
	
	CCamera camera;
	
private:
	int windowWidth;	//Store the width of the window
	int windowHeight;	//Store the height of the window
	float fov, zoom;
	Shader *shader; // GLSL Shader
	
	glm::mat4 ProjectionMatrix;
	glm::mat4 ViewMatrix;
	glm::mat4 ModelMatrix;
	glm::mat4 lightViewMatrix;
	glm::mat4 IdentityMatrix;
	glm::mat4 lightProjectionMatrix;
	
	void calcLightViewMatrix(void);

	int ModelViewMatrixLocation;
	int NormalMatrixLocation;
	int MVPMatrixLocation;
	int samplerLocation;
	int m_ShadowMapLocation;

	bool redisplay;
	
	CObjParser objparser;
	CMesh mesh;
	CMesh plane;
	CMesh skybox;
	std::vector<CLight> light;
	Texture *texture0;
	Texture *texture1;
	Texture *texture2;
	CShadowMapFBO m_ShadowMapFBO;
};


#endif