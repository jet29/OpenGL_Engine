#include "Camera.h"
#include <iostream>

const float MOVEMENT_SPEED = 10.0f;

Camera::Camera(int windowWidth, int windowHeight) :
	viewDirection(0, 0, -1),
	UP(0.0f, 1.0f, 0.0f),
	position(0, 0, 10.82f)
{
	mouseSpeed = 70.0f;
	yaw = 0.0f;
	pitch = 0.0f;
	viewMatrix = glm::lookAt(position, position + viewDirection, UP);
	perspectiveMatrix = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, nearPlane, farPlane);
}

void Camera::mouseUpdate(const glm::vec2& newMousePosition) {
	yaw += newMousePosition.x;
	pitch += newMousePosition.y;
	glm::mat4 Rotation = glm::yawPitchRoll(glm::radians(yaw), glm::radians(pitch), 0.0f);
	viewDirection = glm::vec3(Rotation * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
	UP = glm::vec3(Rotation * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
	//printf("(%f,%f,%f)\n", viewDirection.x, viewDirection.y, viewDirection.z);
	viewMatrix = glm::lookAt(position, position + viewDirection, UP);
}

glm::mat4 Camera::getWorldToViewMatrix() {
	viewMatrix = glm::lookAt(position, position + viewDirection, UP);
	return viewMatrix;
}

glm::mat4 Camera::getPerspectiveMatrix(){
	perspectiveMatrix = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 1.0f, 100.0f);
	return perspectiveMatrix;
}


void Camera::moveForward(float time) {
	float speed = MOVEMENT_SPEED * time;
	position += speed * viewDirection;
	viewMatrix = glm::lookAt(position, position + viewDirection, UP);

}

void Camera::moveBackward(float time) {
	float speed = MOVEMENT_SPEED * time;
	position -= speed * viewDirection;
	//printf("(%f,%f,%f)\n", position.x,position.y,position.z);
	viewMatrix = glm::lookAt(position, position + viewDirection, UP);

}

void Camera::moveLeft(float time) {
	float speed = MOVEMENT_SPEED * time;
	glm::vec3 moveDirection = glm::cross(viewDirection, UP);
	position += -speed * moveDirection;
	//printf("(%f,%f,%f)\n", position.x, position.y, position.z);
	viewMatrix = glm::lookAt(position, position + viewDirection, UP);

}

void Camera::moveRight(float time) {
	float speed = MOVEMENT_SPEED * time;
	glm::vec3 moveDirection = glm::cross(viewDirection, UP);
	position += speed * moveDirection;
	viewMatrix = glm::lookAt(position, position + viewDirection, UP);

}

void Camera::moveUp(float time) {
	float speed = MOVEMENT_SPEED * 0.7 * time;
	position += speed * UP;
	viewMatrix = glm::lookAt(position, position + viewDirection, UP);

}

void Camera::moveDown(float time) {
	float speed = MOVEMENT_SPEED * 0.7 * time;
	position -= speed * UP;
	viewMatrix = glm::lookAt(position, position + viewDirection, UP);

}

void Camera::resize(int windowWidth, int windowHeight) {
	perspectiveMatrix = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, nearPlane, farPlane);
}