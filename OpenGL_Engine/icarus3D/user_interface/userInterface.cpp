#include "UserInterface.h"
#include "../icarus3D.h"
#include <ImGuizmo.h>


int   UI::listbox_item_current = 0;
int   UI::listbox_sobel_gradient = 0;
int   UI::kernelheight = 3;
int   UI::kernelwidth = 3;
float UI::LoG_scale = 0.75f;
bool  UI::b_squareMatrix = 1;
int   UI::res[] = { 0,0 };
int   UI::bpp = 0;
int   UI::dpi = 0;
long  UI::uniqueColors = 0;
bool  UI::hardwareAcceleration = 0;
float UI::f_threshold = 0.5f;
int   UI::i_threshold = 122;
vector<string> items;
static const char* current_item = NULL;

icarus3D* instance;

UI::UI() {
}

bool UI::init(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	if (!ImGui_ImplGlfw_InitForOpenGL(window, true) || !ImGui_ImplOpenGL3_Init("#version 330 core"))
		return false;
	ImGui::StyleColorsDark();

	//Get singleton instance;
	instance = icarus3D::Instance();

	return true;
}

int callback(ImGuiTextEditCallbackData* data) {

	return 0;
}

void UI::mainConfigWindow() {
	ImGui::Begin("Scene creation");


	current_item = items.size() == 0 ? NULL : items[instance->currentScene].c_str();
	// Scene selector
	if (ImGui::BeginCombo("Current scene", current_item)) {
		for (int n = 0; n < items.size(); n++)
		{
			bool is_selected = (current_item == items[n].c_str()); // You can store your selection however you want, outside or inside your objects
			if (ImGui::Selectable(items[n].c_str(), is_selected)) {
				current_item = items[n].c_str();
				instance->currentScene = n;
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
		}
		ImGui::EndCombo();
	}

	if (ImGui::Button("Create New Scene", ImVec2(0, 0))) {
		instance->createScene();
		items.push_back("Scene " + to_string((items.size() + 1)));
	}

	if (instance->currentScene != -1) {
		if (ImGui::CollapsingHeader("Add model", ImGuiTreeNodeFlags_DefaultOpen)) {
			static char buffer[1024] = "assets/models/catscaled.obj";
			ImGui::InputText("Path", buffer, IM_ARRAYSIZE(buffer));
			if (ImGui::Button("Add", ImVec2(0, 0))) {
				instance->addModel(string(buffer));
			}
		}
	}

	ImGui::Separator();

	if (ImGui::Button("Load Scene", ImVec2(0, 0))) {
		instance->loadScene("scene.json");
	}

	if (ImGui::Button("Save Scene", ImVec2(0, 0))) {
		instance->saveScene();
	}
	ImGui::End();
}

void UI::pickedModelWindow() {
	// If there's an element picked show its info
	if (instance->getPickedIndex() != -1) {
		glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)instance->windowWidth / (float)instance->windowHeight, 1.0f, 100.0f);
		glm::mat4 viewMatrix = instance->camera.getWorldToViewMatrix();
		const float* viewMatrixFloat = glm::value_ptr(viewMatrix);
		const float* projectionMatrixFloat = glm::value_ptr(projectionMatrix);
		const float* modelMatrixFloat = glm::value_ptr(glm::mat4(1.0f));
		float** matrix = new float*[4];
		for (int i = 0; i < 4; i++)
			matrix[i] = new float[4];
		ImGuizmo::DrawCube(viewMatrixFloat, projectionMatrixFloat, modelMatrixFloat);
		ImGuizmo::Manipulate(viewMatrixFloat, projectionMatrixFloat, ImGuizmo::ROTATE, ImGuizmo::WORLD, &matrix[0][0]);
		string frameTitle = "Model: " + instance->scene[instance->currentScene]->models[instance->getPickedIndex()]->name;
		ImGui::Begin(frameTitle.c_str(), (bool*)true);
		if (ImGui::IsKeyPressed(GLFW_KEY_T)) {
			cout << "hola" << endl;
		}

		ImGuizmo::TRANSLATE;
		ImGui::End();
	}
}

void UI::directionalLightWindow() {
	ImGui::Begin("Directional light properties");
	float direction[] = { instance->light->properties.direction.x,instance->light->properties.direction.y,instance->light->properties.direction.z };
	ImGui::InputFloat3("Direction",direction,3);
	instance->light->properties.direction = glm::vec3(direction[0], direction[1], direction[2]);
	ImGui::End();

}

void UI::draw() {
	// Start new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
	ImGuizmo::Enable(true);
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);


	// Main configuration window
	mainConfigWindow();

	// Directional light information
	directionalLightWindow();
	
	// Picked model window
	pickedModelWindow();



	
	// Render dear imgui into screen
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::terminate() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
