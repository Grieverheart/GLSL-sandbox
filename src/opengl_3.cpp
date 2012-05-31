#include "../include/opengl_3.h"

OpenGLContext::OpenGLContext(void):
	mesh(1.0f),
	plane(50.),
	skybox(50.0f),
	light0(glm::vec4(0.0, 40.0, 0.0, 1.0))
{
	/////////////////////////////////////////////////
	// Default Constructor for OpenGLContext class //
	/////////////////////////////////////////////////
	zoom = 0.0f;
	fov = 60.0f;
	redisplay = false;
	texture0 = new Texture(GL_TEXTURE_2D, "Textures/crate.bmp");
	texture1 = new Texture(GL_TEXTURE_2D, "Textures/floor-tile.bmp");
	texture2 = new Texture(GL_TEXTURE_2D, "Textures/wall.bmp");
}

OpenGLContext::~OpenGLContext(void) {
	delete texture0;
	delete texture1;
	delete texture2;
	delete shader;
}  

void OpenGLContext::toggleState(std::string state){
	if(state == "redisplay") redisplay = !redisplay;
	else std::cout << "Cannot toggle state " << state << std::endl;
}

bool OpenGLContext::getState(std::string state){
	if(state == "redisplay") return redisplay;
	else std::cout << "Cannot return state " << state << std::endl;
	return false;
}

void OpenGLContext::multMatrix(std::string matrix1, glm::mat4 matrix2){
	if(matrix1 == "ModelMatrix") ModelMatrix *= matrix2;
	else if(matrix1 == "ViewMatrix") ViewMatrix *= matrix2;
}

void OpenGLContext::setMatrix(std::string matrix1, glm::mat4 matrix2){
	if(matrix1 == "ModelMatrix") ModelMatrix = matrix2;
	else if(matrix1 == "ViewMatrix") ViewMatrix = matrix2;
}

bool OpenGLContext::create30Context(void){
	////////////////////////////////////////////////////
	// Create an OpenGL 3.2 context. Fall back to a   //
	// OpenGL 2.1 Context if it fails.				  //
	////////////////////////////////////////////////////
	
	glutInitContextVersion(3,3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ALPHA | GLUT_MULTISAMPLE);
	glutInitWindowSize(600,600);
	windowWidth=windowHeight=600;
	glutInitWindowPosition(100,100);
	glutCreateWindow("Project");
	
	glewExperimental = GL_TRUE;
	GLenum error = glewInit(); //Enable GLEW
	if(error != GLEW_OK) return false; //Failure!
	glError(__FILE__,__LINE__);
	
	int glVersion[2] = {-1,1};
	glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]);
	
	std::cout << "Using OpenGL: " << glVersion[0] << "." << glVersion[1] << std::endl;
	
	return true; // Success, return true
}

void OpenGLContext::setupScene(int argc, char *argv[]){
	glClearColor(0.4f,0.6f,0.9f,1.0f);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	
	shader = new Shader("shaders/shader.vert", "shaders/shader.frag");
	
	MVPMatrixLocation = glGetUniformLocation(shader->id(),"MVPMatrix");
	ModelViewMatrixLocation = glGetUniformLocation(shader->id(),"ModelViewMatrix");
	NormalMatrixLocation = glGetUniformLocation(shader->id(),"NormalMatrix");
	light0.positionLocation = glGetUniformLocation(shader->id(),"light.position");
	samplerLocation = glGetUniformLocation(shader->id(), "inSampler");
	m_ShadowMapLocation = glGetUniformLocation(shader->id(), "inShadowMap");
	
	if(MVPMatrixLocation == -1 || ModelViewMatrixLocation == -1 || NormalMatrixLocation == -1 ||
	   light0.positionLocation == -1 || samplerLocation == -1){
		std::cout << "Unable to bind uniform" << std::endl;
	}
	
	ProjectionMatrix = glm::perspective(fov+zoom, (float)windowWidth/(float)windowHeight, 0.1f, 100.0f);
	ViewMatrix = camera.getView();
	ModelMatrix = glm::mat4(1.0);
	
	objparser.parse("obj/cube-tex.obj", &mesh, "flat");
	mesh.upload(shader->id());
	objparser.parse("obj/plane.obj", &plane, "flat");
	plane.upload(shader->id());
	objparser.parse("obj/cube-tex_inv.obj", &skybox, "flat");
	skybox.upload(shader->id());
	
	if(!texture0->Load()) std::cout << "Couldn't load texture!" << std::endl;
	if(!texture1->Load()) std::cout << "Couldn't load texture!" << std::endl;
	if(!texture2->Load()) std::cout << "Couldn't load texture!" << std::endl;
	if(!m_ShadowMapFBO.Init(windowWidth, windowHeight)) std::cout << "Couldn't initialize FBO!" << std::endl;
}

void OpenGLContext::reshapeWindow(int w, int h){
	windowWidth = w;
	windowHeight = h;
	glViewport(0, 0, windowWidth, windowHeight);
	ProjectionMatrix = glm::perspective(fov+zoom, (float)windowWidth/(float)windowHeight, 0.1f, 100.0f);
}

void OpenGLContext::processScene(void){
	static float last_time = 0.0;
	float this_time = glutGet(GLUT_ELAPSED_TIME)/1000.0f;
	if(this_time - last_time >= 1.0f/61.0f){
		redisplay = true;
		last_time = this_time;
		ProjectionMatrix = glm::perspective(fov+zoom, (float)windowWidth/(float)windowHeight, 0.1f, 100.0f);
		ViewMatrix = camera.getView();
	}
}

void OpenGLContext::fboPass(void){

	m_ShadowMapFBO.BindForWriting();

	glClear(GL_DEPTH_BUFFER_BIT);

	glm::mat4 lightViewMatrix;
	glm::vec3 light_position(-5.0, 10.0, 0.0);
	glm::vec3 light_direction = -glm::normalize(light_position);
	glm::vec3 upVector(0.0,1.0,0.0);
	glm::vec3 sVector;
	sVector = glm::normalize(glm::cross(light_direction, upVector));
	upVector = glm::normalize(glm::cross(sVector, light_direction));
	
	lightViewMatrix = glm::mat4(
		glm::vec4(sVector,glm::dot(-light_position,sVector)),
		glm::vec4(upVector,glm::dot(-light_position,upVector)),
		glm::vec4(-light_direction,glm::dot(light_direction,light_position)),
		glm::vec4(0.0,0.0,0.0,1.0)
	);
	lightViewMatrix = glm::transpose(lightViewMatrix);
	
	glm::mat4 ModelViewMatrix = lightViewMatrix * ModelMatrix;
	glm::mat4 MVPMatrix = ProjectionMatrix * ModelViewMatrix;
	
	{
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(0.0, 1.0, 0.0));
		MVPMatrix = ProjectionMatrix * ModelViewMatrix;
		glUniformMatrix4fv(MVPMatrixLocation, 1, GL_FALSE, &MVPMatrix[0][0]);
		
		mesh.draw();
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLContext::drawPass(void){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// m_ShadowMapFBO.BindForReading(GL_TEXTURE0 + 1);
	// glUniform1i(m_ShadowMapLocation, 1);
	
	texture2->Bind(GL_TEXTURE0 + 0);
	glUniform1i(samplerLocation, 0);
	
	glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
	glm::mat3 NormalMatrix = glm::mat3(glm::transpose(glm::inverse(ModelViewMatrix)));
	glm::mat4 MVPMatrix = ProjectionMatrix * ModelViewMatrix;
	
	glm::vec3 lightEyePosition = glm::vec3(ViewMatrix * glm::vec4(light0.position,1.0));
	glUniform3fv(light0.positionLocation, 1, &lightEyePosition[0]);
	
	glUniformMatrix4fv(MVPMatrixLocation, 1, GL_FALSE, &MVPMatrix[0][0]);
	glUniformMatrix4fv(ModelViewMatrixLocation, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(NormalMatrixLocation, 1, GL_FALSE, &NormalMatrix[0][0]);
	
	skybox.draw();
	texture1->Bind(GL_TEXTURE0 + 0);
	plane.draw();
	{
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(0.0, 1.0, 0.0));
		MVPMatrix = ProjectionMatrix * ModelViewMatrix;
		glUniformMatrix4fv(MVPMatrixLocation, 1, GL_FALSE, &MVPMatrix[0][0]);
		glUniformMatrix4fv(ModelViewMatrixLocation, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		
		texture0->Bind(GL_TEXTURE0 + 0);
		mesh.draw();
	}
}

void OpenGLContext::renderScene(void){
	
	shader->bind();
	{
		fboPass();
		drawPass();
	}
	shader->unbind();
	
	glutSwapBuffers();
}

float OpenGLContext::getZoom(void){
	return zoom;
}

void OpenGLContext::setZoom(float zoom){
	this->zoom = zoom;
}


glm::ivec2 OpenGLContext::getScreen(void){
	glm::ivec2 screen = glm::ivec2(windowWidth, windowHeight);
	return screen;
}