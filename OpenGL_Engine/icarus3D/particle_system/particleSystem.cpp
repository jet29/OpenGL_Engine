#include "particleSystem.h"
#include <iostream>

using namespace std;

ParticleSystem::ParticleSystem(unsigned int _VAO, unsigned int _VBO, glm::vec3 _position, float _spawnRadius) {

	VAO = _VAO;
	VBO = _VBO;

	particles.resize(maxParticles);

	spawnRadius = _spawnRadius;
	positionStart = _position;
	position = positionStart;

	lastParticlePos = -1;

	timeLeftSpawn = timeBetweenSpawn;

	//initialize Seed
	srand(time(NULL));
}


void ParticleSystem::createParticle() {

	int index = (lastParticlePos + 1) % maxParticles;
	
	if (activeParticles < maxParticles) {
		cout << "Creating a new particle number: " <<index<< endl;


		//randomize position start for particle
		float deltaPos_x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / spawnRadius * 2) - spawnRadius);
		float deltaPos_y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / spawnRadius * 2) - spawnRadius);
		float deltaPos_z = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / spawnRadius * 2) - spawnRadius);
		glm::vec3 deltaPos = glm::vec3(deltaPos_x, deltaPos_y, deltaPos_z);

		Particle* particle;
		particle = new Particle(position + deltaPos, glm::vec3(0,1,0));
		particles[index] = particle;
		activeParticles++;
	}
	else {
		particles[index]->reset(position);
	}

	lastParticlePos = index;
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


	// Binds the vertex array to be drawn
	glBindVertexArray(VAO);

	for (int i = 0; i < activeParticles; i++) {


		glm::mat4 model = particles[i]->calculateBillboardMatrix();
		glm::vec2 billboardSize = glm::vec2(1, 1);
		glm::vec3 particleCenter_wordspace = particles[i]->position;

		shader->setVec3("CameraRight_worldspace", CameraRight_worldspace);
		shader->setVec3("CameraUp_worldspace", CameraUp_worldspace);
		shader->setVec3("particleCenter_wordspace", particleCenter_wordspace);
		shader->setVec2("BillboardSize", billboardSize);

		//glm::mat4 model = glm::mat4(1.0f);
		shader->setMat4("model", model);
		particles[i]->draw();
	}

	glBindVertexArray(0);
}

void ParticleSystem::update(float deltaTime){

	spawnParticles(deltaTime);

	for (int i = 0; i < activeParticles; i++) {
		
		particles[i]->update(deltaTime);
	}
}


void ParticleSystem::spawnParticles(float deltaTime) {

	timeLeftSpawn -= deltaTime;

	if (timeLeftSpawn <= 0) {

		for (int i = 0; i < particlesPerSpawn; i++) {

			createParticle();
		}

		timeLeftSpawn = timeBetweenSpawn;
	}
}