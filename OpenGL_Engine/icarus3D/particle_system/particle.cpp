#include "particle.h"
#include <iostream>

using namespace std;


Particle::Particle(glm::vec3 _position, glm::vec3 _dir) {

	ttl = 3;
	speed = 5.0f;

	dir = _dir;
	positionStart = _position;
	position = positionStart;
}

void Particle::draw() {

	// Render triangle's geometry
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	

}


void Particle::update(float deltaTime) {

	position = position + (deltaTime * dir * speed);

}

glm::mat4 Particle::calculateBillboardMatrix() {

	glm::mat4 model = glm::mat4(1.0f);
	glm::vec3 scale = glm::vec3(.5, .5, .5);

	model = glm::translate(model, position);
	//model = glm::scale(model, scale);

	return model;
}

void Particle::reset(glm::vec3 _position) {

	positionStart = _position;
	position = positionStart;
}