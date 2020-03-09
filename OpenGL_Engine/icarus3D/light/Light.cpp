#include "Light.h"



Light::Light() {
	position = glm::vec3(0, 0, 0);
}


Light::~Light() {


}

glm::vec3 Light::setColor(int r, int g, int b) {
	return glm::vec3(r/255.0f, g/255.0f, b/255.0f);
}

glm::vec3 Light::setColor(glm::ivec3 color) {
	return glm::vec3(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f);
}