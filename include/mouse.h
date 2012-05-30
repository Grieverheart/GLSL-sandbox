#ifndef __CMOUSE_H
#define __CMOUSE_H
#include "opengl_3.h"

class CMouse{
public:
	CMouse(OpenGLContext *context);
	~CMouse(void);
	
	void onMouse(int button, int state, int x, int y);
	void onPassiveMotion(int x, int y);
	void centerMouse(void);
	
private:
	OpenGLContext *context;
	int windowWidth;
	int windowHeight;
	int last_mx,last_my;
};
#endif