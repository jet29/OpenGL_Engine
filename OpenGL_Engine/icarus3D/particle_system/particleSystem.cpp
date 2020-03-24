#define _CRT_SECURE_NO_WARNINGS

#include "particleSystem.h"
#include "../json/json.h"
#include <iostream>
#include <fstream>


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
		cout << position.x << endl;
		particle = new Particle(position + deltaPos, particle_direction, particle_speed, particle_ttl);
		particles[index] = particle;
		active_particles++;
	}
	else {
		particles[index]->reset(position + deltaPos, particle_direction, particle_speed, particle_ttl);
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

	//check if max number of particles has change
	if (particles.size() != max_particles) {
		particles.clear();
		active_particles = 0;
		last_particle_index = -1;
		particles.resize(max_particles);
		max_particles = particles.size();
	}

	spawnParticles(deltaTime);

	for (int i = 0; i < active_particles; i++) {
		//UPDATE PARTICLE SPEED
		particles[i]->speed = particle_speed;
		particles[i]->direction = particle_direction;
		particles[i]->scale = particle_scale;

		particles[i]->time_to_live_left -= deltaTime;

		if (particles[i]->time_to_live_left < 0) {

			//randomize position start for particle
			float deltaPos_x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / spawn_radius * 2) - spawn_radius);
			float deltaPos_y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / spawn_radius * 2) - spawn_radius);
			float deltaPos_z = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / spawn_radius * 2) - spawn_radius);
			glm::vec3 deltaPos = glm::vec3(deltaPos_x, deltaPos_y, deltaPos_z);
			particles[i]->reset(position + deltaPos, particle_direction, particle_speed, particle_ttl);
		}

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


bool ParticleSystem::load(const char* path) {

	Json::Value root;
	Json::CharReaderBuilder builder;
	std::ifstream file(path, std::ifstream::binary);
	std::string errs;

	if (!Json::parseFromStream(builder, file, &root, &errs))
	{
		std::cout << errs << "\n";
		return false;
	}

	std::cout << root << endl;


	position.x = root["position"]["x"].asFloat();
	position.y = root["position"]["y"].asFloat();
	position.z = root["position"]["z"].asFloat();

	particle_ttl = root["particle_ttl"].asFloat();
	max_particles = root["max_particles"].asInt();
	particles_per_spawn = root["particles_per_spawn"].asInt();
	spawn_radius = root["spawn_radius"].asFloat();
	time_between_spawn = root["time_between_spawn"].asFloat();
	particle_speed = root["particle_speed"].asFloat();
	particle_scale[0] = root["particle_scale"]["width"].asFloat();
	particle_scale[1] = root["particle_scale"]["height"].asFloat();
	particle_direction[0] = root["particle_direction"]["x"].asFloat();
	particle_direction[1] = root["particle_direction"]["y"].asFloat();
	particle_direction[2] = root["particle_direction"]["z"].asFloat();

	return true;
}

bool ParticleSystem::save(char* name) {

	Json::Value root;


	root["position"]["x"] = position.x;
	root["position"]["y"] = position.y;
	root["position"]["z"] = position.z;

	root["particle_ttl"] = particle_ttl;
	root["max_particles"] = max_particles;
	root["particles_per_spawn"] = particles_per_spawn;
	root["spawn_radius"] = spawn_radius;
	root["time_between_spawn"] = time_between_spawn;
	root["particle_speed"] = particle_speed;
	root["particle_scale"]["width"] = particle_scale[0];
	root["particle_scale"]["height"] = particle_scale[1];
	root["particle_direction"]["x"] = particle_direction[0];
	root["particle_direction"]["y"] = particle_direction[1];
	root["particle_direction"]["z"] = particle_direction[2];

	Json::StreamWriterBuilder builder;
	std::ofstream file_id;

	char* full_path = strcat(name,".json");

	file_id.open(full_path);

	if (!file_id.is_open()) return false;

	file_id << Json::writeString(builder, root);

	file_id.close();

	return true;
}


