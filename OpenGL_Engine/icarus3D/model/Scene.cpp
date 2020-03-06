#include "Scene.h"
#include "../json/json.h"


Scene::Scene() {

}


Scene::~Scene() {


}

bool Scene::addModel(string path) {

	// Push back into scene models
	Model *newModel = new Model();
	newModel->setShader("icarus3D/shaders/basic.vert", "icarus3D/shaders/basic.frag");
	newModel->loadMesh(path.c_str());
	newModel->name = "Name test";
	// Move object to test collisions
	newModel->position = glm::vec3(0, 0, -10);
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

void Scene::removeModel(int index) {

	//models.erase()
}

bool Scene::saveScene() {

    Json::Value root;


    root["name"] = "la mejor escena del mundo";

	for (int i = 0; i < models.size(); i++) {

		Json::Value position;

		glm::vec3 pos = models[i]->position;
		glm::vec3 scale = models[i]->scale;
		glm::vec3 rot = models[i]->rotation;
		
		root["models"][i]["name"] = models[i]->name;
		root["models"][i]["path"] = models[i]->mesh->path;
		
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

	//std::cout << root << endl;

	for (int i = 0; i < root["models"].size(); i++) {
		
		Model* model = new Model();

		model->setShader("icarus3D/shaders/basic.vert", "icarus3D/shaders/basic.frag");
		model->loadMesh(root["models"][i]["path"].asCString());
		
		model->name = root["models"][i]["name"].asCString();

		float x = root["models"][i]["position"]["x"].asFloat();
		float y = root["models"][i]["position"]["y"].asFloat();
		float z = root["models"][i]["position"]["z"].asFloat();

		model->pickingColor = glm::vec3(root["models"][i]["pickingColor"]["r"].asFloat(),
										root["models"][i]["pickingColor"]["g"].asFloat(),
										root["models"][i]["pickingColor"]["b"].asFloat());
							
		printf("(%f,%f,%f)\n", pickingColor.r, pickingColor.g, pickingColor.b);
		cout << x << endl;
		cout << y << endl;
		cout << z << endl;

		model->position = glm::vec3(x, y, z);
		model->rotation = glm::vec3(0, 0, 0);
		model->scale = glm::vec3(1, 1, 1);

		models.push_back(model);
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