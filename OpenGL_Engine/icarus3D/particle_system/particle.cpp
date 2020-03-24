#include "particle.h"
#include <iostream>

using namespace std;


Particle::Particle(glm::vec3 pos, glm::vec3 dir, float spd, float ttl) {

	time_to_live = time_to_live_left = ttl;
	initial_speed = speed = spd;
	initial_direction = direction = glm::normalize(dir);
	initial_position = position = pos;
}

void Particle::draw() {
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Particle::update(float deltaTime) {

	position += deltaTime * direction * speed;
	time_to_live_left -= deltaTime;
	
	if (time_to_live_left < 0) {
		reset();
		time_to_live_left = time_to_live;
	}
	
	model = glm::translate(model, position);
}

void Particle::reset(glm::vec3 new_position, glm::vec3 new_direction, float new_speed, float ttl){
	//if there is a configuration for this particle, change it;
	position = new_position == glm::vec3(0) ? initial_position : new_position;
	direction = new_direction == glm::vec3(0) ? initial_direction : new_direction;
	speed = new_speed == NULL ? initial_speed : new_speed;
	time_to_live = ttl == NULL ? time_to_live : ttl;
	time_to_live_left = time_to_live;
}