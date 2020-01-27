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

	//vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	//vector< vec3 > tempVertices;
	//vector< vec2 > tempUVs;
	//vector< vec3 > tempNormals;

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
			cout << token << endl;
			name = token;
			break;

		case VCOORD:

			glm::vec3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vCoord.push_back(vertex);

			break;
		case NCOORD:

			glm::vec3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			nCoord.push_back(normal);

			break;
		case TCOORD:

			glm::vec2 uv;
			fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
			tCoord.push_back(uv);
			
			break;
		case FACES:

			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf_s(file, "%i/%i/%i %i/%i/%i %i/%i/%i\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				//printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				//return false;
			}

			//printf("f1 (%i,%i,%i)\n\
			//		f2 (%i,%i,%i)\n\
			//	    f3 (%i,%i,%i)\n",
			//	vertexIndex[0], uvIndex[0], normalIndex[0],
			//	vertexIndex[1], uvIndex[1], normalIndex[1],
			//	vertexIndex[2], uvIndex[2], normalIndex[2]);

			fCoord.push_back(vertexIndex[0]);
			fCoord.push_back(vertexIndex[1]);
			fCoord.push_back(vertexIndex[2]);
			
			break;
		}
	}
	return true;
}

bool Model::buildGeometry() {
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(fCoord), &fCoord[0], GL_STATIC_DRAW);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vCoord.size() * sizeof(glm::vec3), &vCoord[0], GL_STATIC_DRAW);

	// Sets the vertex attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindVertexArray(0);


	return true;

	return true;
}

void Model::setShader(const char* vertexPath, const char* fragmentPath){
	delete shader;
	shader = new Shader(vertexPath, fragmentPath);
}