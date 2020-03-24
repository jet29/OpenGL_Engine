#include "DirectionalLight.h"


DirectionalLight::DirectionalLight(){

	properties.direction = glm::vec3(0, -1, 0);
	properties.color.ambient = Light::setColor(237, 249, 6);
	properties.color.diffuse = Light::setColor(255, 255, 255);
	properties.color.specular = Light::setColor(255, 255, 255);

}