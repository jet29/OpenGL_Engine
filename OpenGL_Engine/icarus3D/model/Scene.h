#pragma once
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <map>
#include "Model.h"

using std::vector;
using namespace glm;
using namespace std;


class Scene {
public:

	Scene();

	std::vector<Model> models;

	string saveScene(string path);
	bool loadScene(string path);

	bool addModel(Model model);
	bool addModel(string path);
	void removeModel(int index);

private:
	// initial color for picking
	glm::vec3 pickingColor = { 1.0f,0.0f,0.0f };
	void updatePickingColor();
};