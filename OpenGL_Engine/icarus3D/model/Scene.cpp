#include "Scene.h"


Scene::Scene() {

}

bool Scene::addModel(string path) {

	Model newModel;
	newModel.setShader("icarus3D/shaders/basic.vert", "icarus3D/shaders/basic.frag");
	newModel.loadMesh(path.c_str());

	//move object to test collisions
	newModel.position = glm::vec3(0, 0, -10);

	models.push_back(newModel);

	return true;
}

bool Scene::addModel(Model model) {

	models.push_back(model);

	return true;
}

void Scene::removeModel(int index) {

	//models.erase()
}

string Scene::saveScene(string path) {

}


bool Scene::loadScene(string path) {


}