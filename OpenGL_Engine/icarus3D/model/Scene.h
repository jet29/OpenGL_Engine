#pragma once
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <map>
#include "Model.h"
#include "../light/Light.h"
#include "../light/Pointlight.h"
#include "../light/Spotlight.h"
#include <imgui.hpp>
#include <ImGuizmo.h>

using std::vector;
using namespace glm;
using namespace std;


class Scene {
public:

	Scene();

	~Scene();

	std::vector<Model *> models;
	std::vector<int> pointlight_index;
	string name;
	float DOFThreshold = 0.5f;
	bool saveScene(string path);
	bool loadScene(string path, string name);

	bool addModel(Model *model);
	bool addModel(string pathObj, string pathMtl, string name = "Model");

	bool addLight(string name = "");
	void removeModel(int index);

private:
	// initial color for picking
	glm::vec3 pickingColor = { 1.0f,0.0f,0.0f };
	void updatePickingColor();
};