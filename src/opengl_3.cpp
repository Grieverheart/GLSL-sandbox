#include "../include/opengl_3.h"

OpenGLContext::OpenGLContext(void):
	mesh(1.0f),
	plane(50.),
	skybox(50.0f),
	full_quad(1.0)
{
	/////////////////////////////////////////////////
	// Default Constructor for OpenGLContext class //
	/////////////////////////////////////////////////
	zoom = 0.0f;
	fov = 60.0f;
	znear = 0.1f;
	zfar = 100.0f;
	redisplay = false;
	light.push_back(CLight(glm::vec3(-7.0, 7.0, 0.0), glm::vec3(1.0, -1.0, 0.0)));
	texture0 = new Texture(GL_TEXTURE_2D, "Textures/crate.bmp");
	texture1 = new Texture(GL_TEXTURE_2D, "Textures/floor-tile.bmp");
	texture2 = new Texture(GL_TEXTURE_2D, "Textures/wall.bmp");
	IdentityMatrix = glm::mat4(1.0);
}

OpenGLContext::~OpenGLContext(void) {
	delete texture0;
	delete texture1;
	delete texture2;
	delete sh_gbuffer;
	delete sh_accumulator;
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
	
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ALPHA);
	glutInitWindowSize(600, 600);
	windowWidth=windowHeight = 600;
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Project");
	
	glewExperimental = GL_TRUE;
	GLenum error = glewInit(); //Enable GLEW
	if(error != GLEW_OK) return false; //Failure!
	glError(__FILE__,__LINE__);
	
	int glVersion[2] = {-1,1};
	glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]);
	
	std::cout << "Using OpenGL: " << glVersion[0] << "." << glVersion[1] << std::endl;
	std::cout << "Renderer used: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	return true; // Success, return true
}

void OpenGLContext::setupScene(int argc, char *argv[]){
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_DEPTH_TEST);
	
	sh_gbuffer = new Shader("shaders/gbuffer.vert", "shaders/gbuffer.frag");
	sh_accumulator = new Shader("shaders/accumulator.vert", "shaders/accumulator.frag");
	sh_ssao = new Shader("shaders/ssao.vert", "shaders/ssao.frag");
	sh_blur = new Shader("shaders/blur.vert", "shaders/blur.frag");
	
	// Gbuffer Uniform Locations
	MVPMatrixLocation = glGetUniformLocation(sh_gbuffer->id(),"MVPMatrix");
	NormalMatrixLocation = glGetUniformLocation(sh_gbuffer->id(),"NormalMatrix");
	samplerLocation = glGetUniformLocation(sh_gbuffer->id(), "inSampler");
	
	if(
		MVPMatrixLocation == -1	|| NormalMatrixLocation == -1	||
		samplerLocation == -1
	){ std::cout << "Unable to bind Gbuffer uniforms" << std::endl; }
	
	// Accumulator Uniform Locations
	DepthMapLocation = glGetUniformLocation(sh_accumulator->id(), "DepthMap");
	ColorMapLocation = glGetUniformLocation(sh_accumulator->id(), "ColorMap");
	NormalMapLocation = glGetUniformLocation(sh_accumulator->id(), "NormalMap");
	if(!light[0].Init(sh_accumulator->id())) std::cout << "Cannot bind light uniform" << std::endl;
	projABLocation = glGetUniformLocation(sh_accumulator->id(), "projAB");
	invProjMatrixLocation = glGetUniformLocation(sh_accumulator->id(), "invProjMatrix");
	
	if(
		invProjMatrixLocation == -1	||	DepthMapLocation == -1	||
		ColorMapLocation == -1		||	NormalMapLocation == -1	||
		DepthMapLocation == -1		||	projABLocation == -1
	){ std::cout << "Unable to bind Accumulator uniforms" << std::endl; }
	
	// SSAO Uniform Locations
	ssaoProjMatrixLocation = glGetUniformLocation(sh_ssao->id(), "projectionMatrix");
	ssaoDepthMapLocation = glGetUniformLocation(sh_ssao->id(), "DepthMap");
	ssaoNormalMapLocation = glGetUniformLocation(sh_ssao->id(), "NormalMap");
	ssaoprojABLocation = glGetUniformLocation(sh_ssao->id(), "projAB");
	ssaoinvProjMatrixLocation = glGetUniformLocation(sh_ssao->id(), "invProjMatrix");
	if(!m_ssao.Init(windowWidth, windowHeight, sh_ssao->id())) std::cout << "Couldn't initialize SSAO!" << std::endl;
	
	if(
		ssaoProjMatrixLocation == -1	||	ssaoDepthMapLocation == -1	||
		ssaoNormalMapLocation == -1		||	ssaoprojABLocation == -1	||
		ssaoinvProjMatrixLocation == -1
	){ std::cout << "Unable to bind SSAO main uniforms" << std::endl; }
	
	// Blur Uniform Locations
	aoSamplerLocation = glGetUniformLocation(sh_blur->id(), "aoSampler");
	texelSizeLocation = glGetUniformLocation(sh_blur->id(), "TEXEL_SIZE");
	
	if(
		aoSamplerLocation == -1	||	texelSizeLocation == -1
	){ std::cout << "Unable to bind blur uniforms" << std::endl; }
	
	ProjectionMatrix = glm::perspective(fov+zoom, (float)windowWidth/(float)windowHeight, znear, zfar);
	ViewMatrix = camera.getView();
	ModelMatrix = glm::mat4(1.0);
	
	// Gbuffer Uniforms
	sh_gbuffer->bind();
	{
		glUniform1i(samplerLocation, 0);
	}
	sh_gbuffer->bind();
	
	// Accumulator Uniforms
	glm::vec2 projAB;
	glm::mat4 invProjMatrix;
	
	sh_accumulator->bind();
	{
		glUniform1i(ColorMapLocation, 0);
		glUniform1i(NormalMapLocation, 1);
		glUniform1i(DepthMapLocation, 2);
		float projA = zfar / (zfar - znear);
		float projB = zfar * znear / (zfar - znear);
		projAB = glm::vec2(projA, projB);
		glUniform2fv(projABLocation, 1, &projAB[0]);
		invProjMatrix = glm::inverse(ProjectionMatrix);
		glUniformMatrix4fv(invProjMatrixLocation, 1, GL_FALSE, &invProjMatrix[0][0]);
	}
	sh_accumulator->unbind();
	
	// SSAO Uniforms
	sh_ssao->bind();
	{
		m_ssao.UploadUniforms();
		glUniform1i(ssaoNormalMapLocation, 0);
		glUniform1i(ssaoDepthMapLocation, 1);
		glUniform2fv(ssaoprojABLocation, 1, &projAB[0]);
		glUniformMatrix4fv(ssaoProjMatrixLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);
		glUniformMatrix4fv(ssaoinvProjMatrixLocation, 1, GL_FALSE, &invProjMatrix[0][0]);
		
	}
	sh_ssao->unbind();
	
	// Blur Uniforms
	sh_blur->bind();
	{
		glm::vec2 texel_size = glm::vec2(1.0 / windowWidth, 1.0 / windowHeight);
		glUniform2fv(texelSizeLocation, 1, &texel_size[0]);
		glUniform1i(aoSamplerLocation, 0);
	}
	sh_blur->unbind();
	
	objparser.parse("obj/cube-tex.obj", &mesh, "flat");
	mesh.upload(sh_gbuffer->id());
	objparser.parse("obj/plane.obj", &plane, "flat");
	plane.upload(sh_gbuffer->id());
	objparser.parse("obj/full_quad.obj", &full_quad, "flat");
	full_quad.upload(sh_accumulator->id());
	objparser.parse("obj/cube-tex_inv.obj", &skybox, "flat");
	skybox.upload(sh_gbuffer->id());
	
	if(!texture0->Load()) std::cout << "Couldn't load texture!" << std::endl;
	if(!texture1->Load()) std::cout << "Couldn't load texture!" << std::endl;
	if(!texture2->Load()) std::cout << "Couldn't load texture!" << std::endl;
	if(!m_gbuffer.Init(windowWidth, windowHeight)) std::cout << "Couldn't initialize FBO!" << std::endl;
}

void OpenGLContext::reshapeWindow(int w, int h){
	windowWidth = w;
	windowHeight = h;
	glViewport(0, 0, windowWidth, windowHeight);
	ProjectionMatrix = glm::perspective(fov+zoom, (float)windowWidth/(float)windowHeight, znear, zfar);
}

void OpenGLContext::processScene(void){
	static float last_time = 0.0;
	float this_time = glutGet(GLUT_ELAPSED_TIME)/1000.0f;
	if(this_time - last_time >= 1.0f/61.0f){
		redisplay = true;
		last_time = this_time;
		ProjectionMatrix = glm::perspective(fov+zoom, (float)windowWidth/(float)windowHeight, znear, zfar);
		ViewMatrix = camera.getView();
	}
}

void OpenGLContext::fboPass(void){

	glDisable(GL_BLEND);
	
	m_gbuffer.BindForWriting();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	sh_gbuffer->bind();
	{
		texture2->Bind(GL_TEXTURE0 + 0);
		
		glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
		glm::mat3 NormalMatrix = glm::transpose(glm::inverse(glm::mat3(ModelViewMatrix)));
		glm::mat4 MVPMatrix = ProjectionMatrix * ModelViewMatrix;
		
		glUniformMatrix4fv(MVPMatrixLocation, 1, GL_FALSE, &MVPMatrix[0][0]);
		glUniformMatrix3fv(NormalMatrixLocation, 1, GL_FALSE, &NormalMatrix[0][0]);
		
		skybox.draw();
		texture1->Bind(GL_TEXTURE0 + 0);
		plane.draw();
		{
			ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(0.0, 1.0, 0.0));
			MVPMatrix = ProjectionMatrix * ModelViewMatrix;
			glUniformMatrix4fv(MVPMatrixLocation, 1, GL_FALSE, &MVPMatrix[0][0]);
			
			texture0->Bind(GL_TEXTURE0 + 0);
			mesh.draw();
		}
	}
	sh_gbuffer->unbind();
	
	glEnable(GL_BLEND);
}

void OpenGLContext::ssaoPass(void){
	
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	m_gbuffer.BindForSSAO();
	m_ssao.BindNoise();
	m_ssao.BindForWriting();
	glClear(GL_COLOR_BUFFER_BIT);
	
	sh_ssao->bind();
	{
		full_quad.draw();
	}
	sh_ssao->unbind();
	
	m_ssao.BindForReading();
	glClear(GL_COLOR_BUFFER_BIT);
	m_gbuffer.BindForWriting();
	
	sh_blur->bind();
	{
		full_quad.draw();
	}
	sh_blur->unbind();
	
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
}

void OpenGLContext::drawPass(void){
	
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	m_gbuffer.BindForReading();
	
	sh_accumulator->bind();
	{
		light[0].uploadDirection(ViewMatrix);
		full_quad.draw();
	}
	sh_accumulator->unbind();
	
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
}

void OpenGLContext::renderScene(void){
	
	fboPass();
	ssaoPass();
	drawPass();
	
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