#include "Scene.h"
#include "../json/json.h"


Scene::Scene() {

}

Scene::~Scene() {


}

bool Scene::addModel(string pathObj, string pathMtl, string name) {

	// Push back into scene models
	Model *newModel = new Model();
	// Set shader path to be able to hot reload shaders
	newModel->shaderPath[0] = "icarus3D/shaders/modelShader.vert";
	newModel->shaderPath[1] = "icarus3D/shaders/modelShader.frag";

	newModel->meshPath[0] = pathObj;
	newModel->meshPath[1] = pathMtl;

	// Set shader
	newModel->setShader(newModel->shaderPath[0].c_str(), newModel->shaderPath[1].c_str());
	newModel->loadMesh(pathObj.c_str(), pathMtl.c_str());
	newModel->name = name;
	newModel->type = MODEL;
	newModel->pickingColor = pickingColor;
	// Update picking color
	updatePickingColor();
	
	models.push_back(newModel);
	return true;
}

bool Scene::addModel(Model *model) {

	models.push_back(model);

	return true;
}

bool Scene::addLight(string name) {
	PointLight* newLight = new PointLight();

	newLight->name = name;
	newLight->type = POINTLIGHT;
	// Set shader path to be able to hot reload shaders
	newLight->shaderPath[0] = "icarus3D/shaders/pointlight.vert";
	newLight->shaderPath[1] = "icarus3D/shaders/pointlight.frag";

	newLight->meshPath[0] = "assets/models/light_sphere.obj";
	newLight->meshPath[1] = "assets/models/light_sphere.mtl";
	// Set shader
	newLight->setShader(newLight->shaderPath[0].c_str(), newLight->shaderPath[1].c_str());
	newLight->loadMesh(newLight->meshPath[0].c_str(), newLight->meshPath[1].c_str());
	newLight->pickingColor = pickingColor;
	// Update picking color
	updatePickingColor();
	// Push new light index
	pointlight_index.push_back(models.size());
	// Push light to model list
	models.push_back(newLight);

	return true;
}


void Scene::removeModel(int index) {

	//models.erase()
}

bool Scene::saveScene() {

    Json::Value root;

    root["name"] = "la mejor escena del mundo";

	for (int i = 0; i < models.size(); i++) {

		glm::vec3 pos = models[i]->position;
		glm::vec3 scale = models[i]->scale;
		glm::vec3 rot = models[i]->rotationAngles;
		
		root["models"][i]["name"] = models[i]->name;
		root["models"][i]["type"] = models[i]->type;

		root["models"][i]["shaderPath"][0] = models[i]->shaderPath[0];
		root["models"][i]["shaderPath"][1] = models[i]->shaderPath[1];

		root["models"][i]["meshPath"][0] = models[i]->meshPath[0];
		root["models"][i]["meshPath"][1] = models[i]->meshPath[1];
		
		root["models"][i]["pickingColor"]["r"] = models[i]->pickingColor.r;
		root["models"][i]["pickingColor"]["g"] = models[i]->pickingColor.g;
		root["models"][i]["pickingColor"]["b"] = models[i]->pickingColor.b;

		root["models"][i]["position"]["x"] = pos.x;
		root["models"][i]["position"]["y"] = pos.y;
		root["models"][i]["position"]["z"] = pos.z;

		root["models"][i]["scale"]["x"] = scale.x;
		root["models"][i]["scale"]["y"] = scale.y;
		root["models"][i]["scale"]["z"] = scale.z;

		root["models"][i]["rotation"]["x"] = rot.x;
		root["models"][i]["rotation"]["y"] = rot.y;
		root["models"][i]["rotation"]["z"] = rot.z;

		if (models[i]->type == POINTLIGHT) {
			PointLight* pointlight = (PointLight*)models[i];

			root["models"][i]["color"]["ambient"]["r"] = pointlight->properties.color.ambient.r;
			root["models"][i]["color"]["ambient"]["g"] = pointlight->properties.color.ambient.r;
			root["models"][i]["color"]["ambient"]["b"] = pointlight->properties.color.ambient.r;

			root["models"][i]["color"]["diffuse"]["r"] = pointlight->properties.color.diffuse.r;
			root["models"][i]["color"]["diffuse"]["g"] = pointlight->properties.color.diffuse.r;
			root["models"][i]["color"]["diffuse"]["b"] = pointlight->properties.color.diffuse.r;

			root["models"][i]["color"]["specular"]["r"] = pointlight->properties.color.specular.r;
			root["models"][i]["color"]["specular"]["g"] = pointlight->properties.color.specular.r;
			root["models"][i]["color"]["specular"]["b"] = pointlight->properties.color.specular.r;

			root["models"][i]["attenuation"]["constant"] = pointlight->properties.attenuation.constant;
			root["models"][i]["attenuation"]["linear"] = pointlight->properties.attenuation.linear;
			root["models"][i]["attenuation"]["quadratic"] = pointlight->properties.attenuation.quadratic;
		}
	}

    Json::StreamWriterBuilder builder;
	std::ofstream file_id;
	file_id.open("scene.json");
	
	if (!file_id.is_open()) return false;

	file_id << Json::writeString(builder, root);

	file_id.close();

	return true;
}

bool Scene::loadScene(string path) {


	Json::Value root; 
	Json::CharReaderBuilder builder;
	std::ifstream file(path.c_str(), std::ifstream::binary);
	std::string errs;
	
	if (!Json::parseFromStream(builder, file, &root, &errs))
	{
		std::cout << errs << "\n";
		return false;
	}

	std::cout << root << endl;

	for (int i = 0; i < root["models"].size(); i++) {
		
		Model* model = new Model();

		model->shaderPath[0] = root["models"][i]["shaderPath"][0].asString();
		model->shaderPath[1] = root["models"][i]["shaderPath"][1].asString();

		model->meshPath[0] = root["models"][i]["meshPath"][0].asString();
		model->meshPath[1] = root["models"][i]["meshPath"][1].asString();

		model->setShader(model->shaderPath[0].c_str(), model->shaderPath[1].c_str());
		model->loadMesh(model->meshPath[0].c_str(), model->meshPath[1].c_str());
		
		model->name = root["models"][i]["name"].asCString();

		model->type = (MODEL_TYPE)root["models"][i]["type"].asInt();

		model->pickingColor = glm::vec3(root["models"][i]["pickingColor"]["r"].asFloat(),
										root["models"][i]["pickingColor"]["g"].asFloat(),
										root["models"][i]["pickingColor"]["b"].asFloat());
							

		model->position = glm::vec3(root["models"][i]["position"]["x"].asFloat(),
									root["models"][i]["position"]["y"].asFloat(),
									root["models"][i]["position"]["z"].asFloat());

		model->rotationAngles = glm::vec3(root["models"][i]["rotation"]["x"].asFloat(),
										  root["models"][i]["rotation"]["y"].asFloat(),
										  root["models"][i]["rotation"]["z"].asFloat());

		model->scale = glm::vec3(root["models"][i]["scale"]["x"].asFloat(),
								 root["models"][i]["scale"]["y"].asFloat(),
								 root["models"][i]["scale"]["z"].asFloat());

		model->setTranslationMatrix();
		model->setScaleMatrix();


		if (model->type == POINTLIGHT) {
			PointLight* pointlight = (PointLight*)model;

			pointlight->properties.color.ambient.r = root["models"][i]["color"]["ambient"]["r"].asFloat();
			pointlight->properties.color.ambient.g = root["models"][i]["color"]["ambient"]["g"].asFloat();
			pointlight->properties.color.ambient.b = root["models"][i]["color"]["ambient"]["b"].asFloat();

			pointlight->properties.color.diffuse.r = root["models"][i]["color"]["diffuse"]["r"].asFloat();
			pointlight->properties.color.diffuse.g = root["models"][i]["color"]["diffuse"]["g"].asFloat();
			pointlight->properties.color.diffuse.b = root["models"][i]["color"]["diffuse"]["b"].asFloat();

			pointlight->properties.color.specular.r = root["models"][i]["color"]["specular"]["r"].asFloat();
			pointlight->properties.color.specular.g = root["models"][i]["color"]["specular"]["g"].asFloat();
			pointlight->properties.color.specular.b = root["models"][i]["color"]["specular"]["b"].asFloat();

			pointlight->properties.attenuation.constant = root["models"][i]["attenuation"]["constant"].asFloat();
			pointlight->properties.attenuation.linear = root["models"][i]["attenuation"]["linear"].asFloat();
			pointlight->properties.attenuation.quadratic = root["models"][i]["attenuation"]["quadratic"].asFloat();

			pointlight_index.push_back(models.size());

			models.push_back(pointlight);
		}
		else {
			models.push_back(model);
		}
		
	}

	// Assign global color
	pickingColor = glm::vec3(root["models"][root["models"].size() - 1]["pickingColor"]["r"].asFloat()-0.01f,
							 root["models"][root["models"].size() - 1]["pickingColor"]["g"].asFloat(),
							 root["models"][root["models"].size() - 1]["pickingColor"]["b"].asFloat());


	return true;
}

void Scene::updatePickingColor() {
	if (pickingColor.r != 0.0f)
		pickingColor.r -= 0.01f;
	else if (pickingColor.g != 0.0f)
		pickingColor.g -= 0.01f;
	else if (pickingColor.b != 0.0f)
		pickingColor.b -= 0.01f;
}