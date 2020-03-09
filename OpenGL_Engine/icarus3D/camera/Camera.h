#pragma once
#include <glm\glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

class Camera
{
public:
	glm::vec3 position;
	glm::vec3 viewDirection;
	glm::vec3 UP;
	glm::vec2 oldMousePosition;
	float mouseSpeed;
	float yaw, pitch;
	Camera();
	glm::mat4 getWorldToViewMatrix() const;
	glm::mat4 getPerspectiveMatrix() const;
	void mouseUpdate(const glm::vec2& newMousePosition);
	void moveForward(float time);
	void moveBackward(float time);
	void moveLeft(float time);
	void moveRight(float time);
	void moveUp(float time);
	void moveDown(float time);
};
