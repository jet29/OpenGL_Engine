#pragma once
#include "Light.h"

class DirectionalLight : public Light {

public:

	DirectionalLight();

	struct DirectionalLightProperties {
		glm::vec3 direction;
		LightColor color;
	} properties;


};