#include "PointLight.h"


PointLight::PointLight(){
	shaderPath[0] = "";
	shaderPath[1] = "";
	properties.color.ambient = Light::setColor(glm::ivec3(255, 0, 0));
	properties.color.diffuse = Light::setColor(glm::ivec3(0, 255, 0));
	properties.color.specular = Light::setColor(glm::ivec3(0, 0, 255));
	properties.attenuation.constant = 0.956f;
	properties.attenuation.linear = 0.118f;
	properties.attenuation.quadratic = 0.0f;
}