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
	vec3 vCoords;
	vec3 nCoords;
	vec2 tCoords;

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
			break;
		case VCOORD:
			break;
		case NCOORD:
			break;
		case TCOORD:
			break;
		case FACES:
			break;
		}
	}

}