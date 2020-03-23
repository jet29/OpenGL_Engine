#include "particleSystem.h"
#include <iostream>

using namespace std;

ParticleSystem::ParticleSystem(unsigned int _VAO, unsigned int _VBO, unsigned int seed) {

	VAO = _VAO;
	VBO = _VBO;

	particles.resize(max_particles);
	position = initial_position;
	time_left_to_spawn = time_between_spawn;
	
	//initialize random generator with seed
	srand(seed);
}


void ParticleSystem::createParticle() {

	int index = (last_particle_index + 1) % max_particles;

	//randomize position start for particle
	float deltaPos_x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / spawn_radius * 2) - spawn_radius);
	float deltaPos_y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / spawn_radius * 2) - spawn_radius);
	float deltaPos_z = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / spawn_radius * 2) - spawn_radius);
	glm::vec3 deltaPos = glm::vec3(deltaPos_x, deltaPos_y, deltaPos_z);
	
	if (active_particles < max_particles) {
		Particle* particle;
		particle = new Particle(position + deltaPos, particle_direction, particle_speed, 3.0f);
		particles[index] = particle;
		active_particles++;
	}
	else {
		particles[index]->reset(position + deltaPos, particle_direction, particle_speed);
	}

	last_particle_index = index;
}



void ParticleSystem::draw(Shader* shader, GLuint texture, glm::mat4 view, glm::mat4 projection) {

	shader->use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	shader->setInt("image", 0);

	shader->setMat4("view", view);
	shader->setMat4("projection", projection);

	glm::vec3 CameraRight_worldspace;
	glm::vec3 CameraUp_worldspace;

	CameraRight_worldspace = glm::vec3(view[0][0], view[1][0], view[2][0]);
	CameraUp_worldspace = glm::vec3(view[0][1], view[1][1], view[2][1]);

	glBindVertexArray(VAO);

	for (int i = 0; i < active_particles; i++) {

		glm::vec2 billboardSize = particles[i]->scale;
		glm::vec3 particleCenter_wordspace = particles[i]->position;

		shader->setVec3("CameraRight_worldspace", CameraRight_worldspace);
		shader->setVec3("CameraUp_worldspace", CameraUp_worldspace);
		shader->setVec3("particleCenter_worldspace", particleCenter_wordspace);
		shader->setVec2("BillboardSize", billboardSize);

		//shader->setMat4("model", model); //is not used
		particles[i]->draw();
	}

	glBindVertexArray(0);
}

void ParticleSystem::update(float deltaTime){

	spawnParticles(deltaTime);

	for (int i = 0; i < active_particles; i++) {
		//UPDATE PARTICLE SPEED
		particles[i]->speed = particle_speed;
		particles[i]->direction = particle_direction;
		particles[i]->scale = particle_scale;
		particles[i]->update(deltaTime);
	}
		
}


void ParticleSystem::spawnParticles(float deltaTime) {

	time_left_to_spawn -= deltaTime;

	if (time_left_to_spawn <= 0) { //time to spawn

		for (int i = 0; i < particles_per_spawn; i++) 
			createParticle();

		time_left_to_spawn = time_between_spawn; //reset spawn counter
	}
}