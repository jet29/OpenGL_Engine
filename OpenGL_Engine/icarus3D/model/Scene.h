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

	~Scene();

	std::vector<Model *> models;

	bool saveScene();
	bool loadScene(string path);

	bool addModel(Model *model);
	bool addModel(string path);
	void removeModel(int index);

private:

};