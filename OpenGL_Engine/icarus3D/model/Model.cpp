#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Mesh class functions declarations

//std::map<string, MeshData*> Mesh::s_resourceMap;


Model::Model() {

}

void Model::setShader(const char* vertexPath, const char* fragmentPath){
	delete shader;
	shader = new Shader(vertexPath, fragmentPath);
}

void Model::loadMesh(string path) {
	mesh = new Mesh(path);
}