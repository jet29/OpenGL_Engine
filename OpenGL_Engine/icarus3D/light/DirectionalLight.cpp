#include "DirectionalLight.h"


DirectionalLight::DirectionalLight(){

	properties.direction = glm::vec3(0, -1, 0);
	properties.color.ambient = glm::vec3(0, 0, 1);
	properties.color.specular = glm::vec3(1, 1, 1);
	properties.color.diffuse = glm::vec3(1, 0, 0);

}