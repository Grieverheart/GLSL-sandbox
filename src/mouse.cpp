#include "../include/mouse.h"
#include "../include/main.h"

CMouse::CMouse(OpenGLContext *context){
	this->context = context;
}

CMouse::~CMouse(void){
}

void CMouse::centerMouse(void){
	glm::ivec2 screen = context->getScreen();
	windowWidth = screen.x;
	windowHeight = screen.y;
	last_my = windowHeight/2, last_mx = windowWidth/2;
	glutWarpPointer(last_mx,last_my);
}

void CMouse::onMouse(int button, int state, int x, int y){
	if(button==3||button==4){
		float zoom = context->getZoom();
		zoom=zoom+(button*2-7)*1.5f; // put wheel up and down in one
		if(zoom<-58)zoom=-58.0f;
		else if(zoom>90)zoom=90.0f;
		context->setZoom(zoom);
	}
}

void CMouse::onPassiveMotion(int x, int y){
	if(x != last_mx || y != last_my){
		float dx = (float)(x - last_mx)/windowWidth;
		float dy = (float)(last_my - y)/windowHeight;
		context->camera.rotate(dx,dy);
		glutWarpPointer(last_mx,last_my);
	}
}
