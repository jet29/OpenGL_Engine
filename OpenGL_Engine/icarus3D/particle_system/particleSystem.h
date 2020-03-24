#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glad\glad.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */


#include "particle.h"

#include <vector>

using namespace std;


class ParticleSystem {


public:
	
	ParticleSystem(unsigned int _VAO, unsigned int _VBO, unsigned int seed);

	void createParticle();

	void spawnParticles(float deltaTime);

	void update(float deltaTime);

	void draw(Shader* shader, GLuint texture, glm::mat4 view, glm::mat4 projection);

	bool load(const char* path);
	bool save(char* name);

	//CONFIGURABLE PARAMETERS PARTICLE SYSTEM
	int max_particles = 1000;
	int particles_per_spawn = 10;
	float spawn_radius = 10;
	float time_between_spawn = 1;

	//CONFIGURABLE PARAMETERS FOR PARTICLES
	float particle_speed = 1;
	float particle_ttl = 3.0;
	glm::vec3 particle_direction = glm::vec3(0, 1, 0);
	glm::vec2 particle_scale = glm::vec2(1, 1);

	
private:
	float time_left_to_spawn;
	int active_particles = 0;
	int last_particle_index = -1; 
	vector<Particle*> particles;


	glm::vec3 position;
	glm::vec3 initial_position = glm::vec3(0.0f);

	unsigned int VBO;
	unsigned int VAO;


};