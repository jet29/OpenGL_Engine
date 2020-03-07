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

void UI::draw() {
	// Start new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
	// Set ImGui parameters
	// Techniques list
	ImGui::Begin("Scene creation");
	if (ImGui::Button("Add model", ImVec2(0, 0))) {
		instance->addModel();
	}

	if (ImGui::Button("Create New Scene", ImVec2(0, 0))) {
		instance->createScene();
	}

	if (ImGui::Button("Load Scene", ImVec2(0, 0))) {
		instance->loadScene("scene.json");
	}

	if (ImGui::Button("Save Scene", ImVec2(0, 0))) {
		instance->saveScene();
	}
	ImGui::End();
	// If there's an element picked show its info
	if (instance->getPickedIndex() != -1) {

		glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)instance->windowWidth / (float)instance->windowHeight, 1.0f, 100.0f);
		glm::mat4 viewMatrix = instance->camera.getWorldToViewMatrix();

		ImGuizmo::DrawCube(glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix), glm::value_ptr(glm::mat4(1.0f)));
		ImGui::Begin("Test");
		char* buffer = new char(255);
		ImGui::Text("Model Name: %s",instance->scene->models[instance->getPickedIndex()]->name.c_str());
		ImGuizmo::TRANSLATE;
		ImGui::End();
	}
	
	// Render dear imgui into screen
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::terminate() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
