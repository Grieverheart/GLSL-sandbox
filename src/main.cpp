#include "../include/main.h"
#include "../include/opengl_3.h"
#include "../include/keyboard.h"
#include "../include/mouse.h"

OpenGLContext openglContext; // Our OpenGL Context Object
CKeyboard keyboard(&openglContext);
CMouse mouse(&openglContext);
////////////GLUT Keyboard Function Wrappers/////////////
void keyDown(unsigned char key, int x, int y){
	keyboard.keyIsDown(key);
	keyboard.keyOps();
}

void keyUp(unsigned char key, int x, int y){
	keyboard.keyIsUp(key);
}

void specialDown(int key, int x, int y){
	keyboard.keySpIsDown(key);
	keyboard.keySpOps();
}

void specialUp(int key, int x, int y){
	keyboard.keySpIsUp(key);
}

//////////////GLUT Mouse Function Wrappers////////////////

void onMouse(int button, int state, int x, int y){
	mouse.onMouse(button,state,x,y);
}

void onPassiveMotion(int x, int y){
	mouse.onPassiveMotion(x,y);
}
//////////////////////////////////////////////////////////

void display(void){
	openglContext.renderScene();
}

void idle(void){
	openglContext.processScene();
	if(openglContext.getState("redisplay")){
		openglContext.toggleState("redisplay");
		glutPostRedisplay();
	}
}

void reshape(int width, int height){
	openglContext.reshapeWindow(width,height);
	mouse.centerMouse();
}

void init(int argc, char *argv[]){
	openglContext.create30Context();
	mouse.centerMouse();
	openglContext.setupScene(argc,argv);
}

int main(int argc,char *argv[] ){

	glutInit(&argc,argv);
	
	init(argc,argv);
	
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	
	
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);
	glutSpecialFunc(specialDown);
	glutSpecialUpFunc(specialUp);
	glutMouseFunc(onMouse);
	glutPassiveMotionFunc(onPassiveMotion);
	
	glutMainLoop();
	
	return 1;
}