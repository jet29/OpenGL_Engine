#pragma once
#include "../model/Model.h"
#include "Light.h"

class PointLight : public Light{

public:

	PointLight();
	
	struct PointLightProperties {
		glm::vec3 position;
		LightColor color;
		Attenuation attenuation;
	} properties;

private:

};





