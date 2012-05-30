#ifndef __OPENGL_3_H
#define __OPENGL_3_H

#include <string>
#include "main.h"
#include "shader.h"
#include "obj_parser.h"
#include "mesh.h"
#include "camera.h"
#include "texture.h"

struct Light{
	glm::vec3 position;
	int positionLocation;
	Light(glm::vec4 position):
		position(position)
	{};
	Light(void):
		position(glm::vec4(100.0, 40.0, 100.0, 1.0))
	{};
	~Light(void){};
};

class OpenGLContext{
public:
	OpenGLContext(void); 				// Default constructor  
	~OpenGLContext(void); 				// Destructor for cleaning up our application  
	bool create30Context(void); 	// Creation of our OpenGL 3.x context  
	void setupScene(int argc, char *argv[]); 				// All scene information can be setup here  
	void reshapeWindow(int w, int h); 	// Method to get our window width and height on resize  
	void renderScene(void); 			// Render scene (display method from previous OpenGL tutorials)
	void processScene(void);
	
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

	int ModelViewMatrixLocation;
	int NormalMatrixLocation;
	int MVPMatrixLocation;
	int samplerLocation;

	bool redisplay;
	
	CObjParser objparser;
	CMesh mesh;
	CMesh plane;
	CMesh skybox;
	Light light0;
	Texture *texture0;
	Texture *texture1;
	Texture *texture2;
};


#endif