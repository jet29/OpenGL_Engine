#pragma once
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <map>
#include "../model/Model.h"

struct Attenuation {
	float constant;
	float linear;
	float quadratic;
};

struct LightColor {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};



class Light : public Model{

private:

public:
	static glm::vec3 setColor(int r, int g, int b);
	static glm::vec3 setColor(glm::ivec3 color);
	bool lightSwitch = true;
	Light();

	~Light();
};