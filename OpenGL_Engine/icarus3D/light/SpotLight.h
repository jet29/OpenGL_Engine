#pragma once
#include "Light.h"

class SpotLight : Light {

public:

	SpotLight();

	struct SpotLightProperties {
		glm::vec3 position;
		glm::vec3 direction;
		LightColor color;
		float cutOff;
		float outerCutOff;
		Attenuation attenuation;
	} properties;

private:
};