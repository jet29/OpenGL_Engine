#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glad\glad.h>
#include "../Shader.h"

class Particle {

public:

	Particle(glm::vec3 pos, glm::vec3 dir, float spd, float ttl);

	void reset(glm::vec3 new_position = glm::vec3(0), glm::vec3 new_direction = glm::vec3(0), float new_speed = NULL);

	void update(float deltaTime);
	void draw();
	
	glm::mat4 model = glm::mat4(1.0f);

	//time to live
	float time_to_live;

	glm::vec3 position;
	glm::vec2 scale = glm::vec2(1.0f);
	glm::vec3 direction;
	float speed;

	glm::vec3 initial_position;
	glm::vec3 initial_direction;
	float initial_speed;

};
