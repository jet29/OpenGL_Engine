#include "Model.h"

Model::Model() {

}

int Model::getToken(string token) {
	if (token == "o")
		return NEW_OBJECT;
	if (token == "v")
		return VCOORD;
	if (token == "vt")
		return NCOORD;
	if (token == "vn")
		return TCOORD;
	if (token == "f")
		return FACES;
	return NONE;
}

bool Model::loadOBJ(const char* path) {
	FILE* file;
	fopen_s(&file, path, "r");
	char token[255];
	int check;
	Model *part = new Model();
	//Verify a valid path
	if (file == NULL) {
		cout<< "ERROR::Impossible to open model "<<path<<endl;
		return false;
	}

	while (true) {
		check = fscanf_s(file, "%s", token, sizeof(token));
		//Exit condition:
		if (check == EOF)
			break;
		check = getToken(token);
		switch (check) {
		case NEW_OBJECT:
			fscanf_s(file, "%s", token, sizeof(token));
			part = new Model();
			part->name = token;
			break;

		case VCOORD:
			glm::vec3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			part->vCoord.push_back(vertex);

			break;
		case NCOORD:

			glm::vec3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			part->nCoord.push_back(normal);

			break;
		case TCOORD:

			glm::vec2 uv;
			fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
			part->tCoord.push_back(uv);
			break;
		case FACES:

			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf_s(file, "%i/%i/%i %i/%i/%i %i/%i/%i\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("ERROR::File can't be read by our parser: Try exporting with other options\n");
				return false;
			}

			part->fCoord.push_back(vertexIndex[0]-1);
			part->fCoord.push_back(vertexIndex[1]-1);
			part->fCoord.push_back(vertexIndex[2]-1);
			break;
		}
	}

	// Push a new part
	parts.push_back(part);

	return true;
}

bool Model::buildGeometry() {
	// For each model part build it's geometry
	std::vector<Model*>::iterator it = parts.begin();
	for (it; it != parts.end(); it++) {
		// Generate buffers
		glGenVertexArrays(1, &(*it)->VAO);
		glGenBuffers(1, &(*it)->VBO);
		glGenBuffers(1, &(*it)->EBO);

		// Bind VAO
		glBindVertexArray((*it)->VAO);

		// Bind VBO
		glBindBuffer(GL_ARRAY_BUFFER, (*it)->VBO);
		glBufferData(GL_ARRAY_BUFFER, (*it)->vCoord.size() * sizeof(glm::vec3), &(*it)->vCoord[0], GL_STATIC_DRAW);
		
		// Bind EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*it)->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (*it)->fCoord.size() * sizeof(int), &(*it)->fCoord[0], GL_STATIC_DRAW);

		// Set vertex attributes
		// Vertex position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	return true;
}

void Model::setShader(const char* vertexPath, const char* fragmentPath){
	delete shader;
	shader = new Shader(vertexPath, fragmentPath);
}