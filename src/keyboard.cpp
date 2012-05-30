#include "../include/keyboard.h"
#include "../include/camera.h"

CKeyboard::CKeyboard(OpenGLContext *context){
	keyRepeat = false;
	fullscreen = false;
	this->context = context;
}

CKeyboard::~CKeyboard(void){
}

void CKeyboard::keyIsDown(int key){
	keyMap[key]=true;
}

void CKeyboard::keyIsUp(int key){
	keyMap[key]=false;
}

void CKeyboard::keySpIsDown(int key){
	keySpMap[key]=true;
}

void CKeyboard::keySpIsUp(int key){
	keySpMap[key]=false;
}

void CKeyboard::keyOps(void){
	if(keyMap[27])exit(0);//exit when escape
	
	if(keyMap[13]){ //Enter
		int mod=glutGetModifiers();
		if(mod==GLUT_ACTIVE_ALT){
			if(fullscreen){
				glutReshapeWindow(600,600);
				glutSetWindowTitle("Project");
			}
			else{
				glutFullScreen();
				glutSetWindowTitle("Project Fullscreen");
			}
			fullscreen=!fullscreen;
		}
	}
	
	if(keyMap['w']){
		context->camera.translate(CAM_FW);
	}
	
	if(keyMap['s']){
		context->camera.translate(CAM_BW);
	}
	
	if(keyMap['q']){
		context->camera.translate(CAM_LEFT);
	}
	
	if(keyMap['e']){
		context->camera.translate(CAM_RIGHT);
	}
	
	if(keyMap['a']){
		context->camera.rotate(-0.01f,0.0f);
	}
	
	if(keyMap['d']){
		context->camera.rotate(0.01f,0.0f);
	}
	
	if(keyMap[32]){ //spacebar
		context->camera.translate(CAM_JUMP);
	}
}

void CKeyboard::keySpOps(void){
	if(keySpMap[GLUT_KEY_F1]){
		if(!keyRepeat)printf("Key Repeat Enabled\n");
		else printf("Key Repeat Disabled\n");
		keyRepeat=!keyRepeat;
		glutSetKeyRepeat(keyRepeat);
	}
}