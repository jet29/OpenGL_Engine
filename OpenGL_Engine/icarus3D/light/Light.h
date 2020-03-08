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



class Light : Model{

private:

public:

	glm::vec3 position;
	static glm::vec3 setColor(int r, int g, int b);
	Light();

	~Light();
};