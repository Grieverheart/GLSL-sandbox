#ifndef __CAMERA_H
#define __CAMERA_H
#include "main.h"

#define CAM_FW 0
#define CAM_BW 1
#define CAM_LEFT 2
#define CAM_RIGHT 3
#define CAM_JUMP 4

class CCamera{
public:
	CCamera(void);
	~CCamera(void);
	
	void rotate(float dx, float dy);
	void setPosition(glm::vec3 position);
	void translate(int dir);
	glm::mat4 getView(void);
	
private:
	bool update;
	bool animation;
	bool jump_animation;
	unsigned int frame;
	unsigned int jump_frame;
	glm::vec3 tr_direction;
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 upVector;
	glm::vec3 sVector;
	glm::mat4 viewMatrix;
};

#endif