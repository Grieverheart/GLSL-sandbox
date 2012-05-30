#include "../include/camera.h"

CCamera::CCamera(void){
	update = true;
	animation = false;
	jump_animation = false;
	upVector = glm::vec3(0.0, 1.0, 0.0);
	sVector = glm::vec3(1.0, 0.0, 0.0);
	direction = glm::vec3(0.0, 0.0, -1.0);
	position = glm::vec3(0.0, 4.0, 10.0);
	tr_direction = glm::vec3(0.0);
	viewMatrix = glm::mat4(glm::vec4(sVector,glm::dot(-position,sVector)), glm::vec4(upVector,glm::dot(-position,upVector)), glm::vec4(-direction,glm::dot(direction,position)),glm::vec4(0.0,0.0,0.0,1.0));
	viewMatrix = glm::transpose(viewMatrix);
}

CCamera::~CCamera(void){
}

void CCamera::rotate(float dx, float dy){
	glm::mat3 rotationUp = glm::mat3(glm::rotate(glm::mat4(1.0),-180.0f*dx,glm::vec3(0.0, 1.0, 0.0)));
	glm::mat3 rotationS = glm::mat3(glm::rotate(glm::mat4(1.0),180.0f*dy,sVector));
	glm::vec3 temp_direction = rotationUp * rotationS * direction;
	sVector = rotationUp * sVector;
	sVector = glm::normalize(sVector);
	glm::vec3 temp_upVector = glm::cross(sVector, temp_direction);
	if(temp_upVector.y > 0.0){
		upVector = glm::normalize(temp_upVector);
		direction = glm::normalize(temp_direction);
	}
	else{
		direction = rotationUp * direction;
		direction = glm::normalize(direction);
		upVector = glm::cross(sVector, direction);
		upVector = glm::normalize(upVector);
	}
	update = true;
}

void CCamera::setPosition(glm::vec3 position){
	this->position = position;
	update = true;
}

void CCamera::translate(int dir){
	if(dir == CAM_FW) tr_direction += glm::vec3(direction.x, 0.0, direction.z);
	else if(dir == CAM_BW) tr_direction += -glm::vec3(direction.x, 0.0, direction.z);
	else if(dir == CAM_LEFT) tr_direction += -sVector;
	else if(dir == CAM_RIGHT) tr_direction += sVector;
	else{
		jump_animation = true;
		update = true;
		return;
	}
	frame = 0;
	if(glm::length(tr_direction) != 0.0f) tr_direction = glm::normalize(tr_direction);
	update = true;
	animation = true;
}

glm::mat4 CCamera::getView(void){
	if(jump_animation){
		position.y = -0.007f*(jump_frame*jump_frame)/2.0f + (1.0f/6.0f) * jump_frame + 4.0f;
		jump_frame++;
		if(position.y < 4.0f){
			position.y = 4.0f;
			jump_frame = 0;
			jump_animation = false;
		}
		update = true;
	}
	if(animation){
		float acc = -0.007;
		if(jump_animation) acc = 0.0f;
		position += (frame * acc + acc/2.0f + 1.0f/6.0f) * tr_direction;
		frame++;
		float velocity = acc * frame + 1.0f/6.0f;
		if(velocity < 0.0f){
			tr_direction = glm::vec3(0.0);
			animation = false;
			frame = 0;
		}
		update = true;
	}
	if(update){
		viewMatrix = glm::mat4(glm::vec4(sVector,glm::dot(-position,sVector)), glm::vec4(upVector,glm::dot(-position,upVector)), glm::vec4(-direction,glm::dot(direction,position)),glm::vec4(0.0,0.0,0.0,1.0));
		viewMatrix = glm::transpose(viewMatrix);
		update = false;
	}
	return viewMatrix;
}