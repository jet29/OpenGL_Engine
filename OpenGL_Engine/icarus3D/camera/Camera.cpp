#include "Camera.h"
#include <iostream>

const float MOVEMENT_SPEED = 25.0f;

Camera::Camera() :
	viewDirection(0, 0, -1),
	UP(0.0f, 1.0f, 0.0f),
	position(0, 0, 0)
{
	mouseSpeed = 70.0f;
	yaw = 0.0f;
	pitch = 0.0f;
}

void Camera::mouseUpdate(const glm::vec2& newMousePosition) {
	yaw += newMousePosition.x;
	pitch += newMousePosition.y;
	glm::mat4 Rotation = glm::yawPitchRoll(glm::radians(yaw), glm::radians(pitch), 0.0f);
	viewDirection = glm::vec3(Rotation * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
	UP = glm::vec3(Rotation * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
}

glm::mat4 Camera::getWorldToViewMatrix() const {
	//printf("position (%f,%f,%f) viewDiw (%f,%f,%f)\n", position[0], position[1], position[2], viewDirection[0], viewDirection[1], viewDirection[2]);
	return glm::lookAt(position, position + viewDirection, UP);
}

glm::mat4 Camera::getPerspectiveMatrix()const{
	return glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 1.0f, 100.0f);
}


void Camera::moveForward(float time) {
	float speed = MOVEMENT_SPEED * time;
	position += speed * viewDirection;
}

void Camera::moveBackward(float time) {
	float speed = MOVEMENT_SPEED * time;
	position -= speed * viewDirection;
}

void Camera::moveLeft(float time) {
	float speed = MOVEMENT_SPEED * time;
	glm::vec3 moveDirection = glm::cross(viewDirection, UP);
	position += -speed * moveDirection;
}

void Camera::moveRight(float time) {
	float speed = MOVEMENT_SPEED * time;
	glm::vec3 moveDirection = glm::cross(viewDirection, UP);
	position += speed * moveDirection;
}

void Camera::moveUp(float time) {
	float speed = MOVEMENT_SPEED * time;
	position += speed * UP;
}

void Camera::moveDown(float time) {
	float speed = MOVEMENT_SPEED * time;
	position -= speed * UP;
}