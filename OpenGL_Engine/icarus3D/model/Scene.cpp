#include "Scene.h"


Scene::Scene() {

}

bool Scene::addModel(string path) {

	Model newModel;
	newModel.setShader("icarus3D/shaders/basic.vert", "icarus3D/shaders/basic.frag");
	newModel.loadMesh(path.c_str());
	newModel.name = "MALDITA SEAAAA";
	//move object to test collisions
	newModel.position = glm::vec3(0, 0, -10);
	newModel.pickingColor = pickingColor;
	// Update picking color
	updatePickingColor();
	// Push back into scene models
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


	return "";
}


bool Scene::loadScene(string path) {

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