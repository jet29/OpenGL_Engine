#pragma once
#include "Light.h"


class PointLight : Light {

public:

	PointLight();
	
	struct PointLightProperties {
		glm::vec3 position;
		LightColor color;
		Attenuation attenuation;
	} properties;

private:

};





