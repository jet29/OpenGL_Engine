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
static const char* current_item_scene = NULL;
static const char* current_item_light = NULL;
static const char* current_item_light_selector = NULL;

vector<string> lights = { "Directional", "Pointlight" };

icarus3D* instance;

UI::UI() {
}

bool UI::init(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	if (!ImGui_ImplGlfw_InitForOpenGL(window, true) || !ImGui_ImplOpenGL3_Init("#version 330 core"))
		return false;
	ImGui::StyleColorsDark();

	//Get singleton instance;
	instance = icarus3D::Instance();
	return true;
}

void UI::settingsWindow() {
	if (settingFlag) {
		ImGui::Begin("Settings");

		current_item_scene = instance->scene.size() == 0 ? NULL : instance->scene[instance->currentScene]->name.c_str();
		// Scene selector
		ImGui::Text("Current scene");
		if (ImGui::BeginCombo("##combo_scenes", current_item_scene)) {
			for (int n = 0; n < instance->scene.size(); n++)
			{
				bool is_selected = (current_item_scene == instance->scene[n]->name.c_str()); // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(instance->scene[n]->name.c_str(), is_selected)) {
					current_item_scene = instance->scene[n]->name.c_str();
					instance->currentScene = n;
					instance->setPickedIndex(-1);
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}
			ImGui::EndCombo();
		}

		ImGui::DragFloat("DOF Threshold", &instance->scene[instance->currentScene]->DOFThreshold, 0.005f, 0, 100);

		ImGui::Separator();

		// Light selector
		string test;
		ImGui::Text("Directional Light Properties");
		if (ImGui::BeginCombo("##combo_lights", current_item_light)){
			for (int n = 0; n < 2; n++)
			{
				bool is_selected2 = (current_item_light == lights[n].c_str()); // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(lights[n].c_str(), is_selected2)) {
					current_item_light = lights[n].c_str();
				}
				if (is_selected2)
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}
			ImGui::EndCombo();
		}
		directionalLightProperties();
		pointLightProperties();
		//if (current_item_light == "Pointlight")
			

		ImGui::End();
	}
}

void UI::directionalLightProperties() {
	ImGui::RadioButton("X", &dirLight_dir_radioButtons_opt, 0); ImGui::SameLine();
	ImGui::RadioButton("Y", &dirLight_dir_radioButtons_opt, 1); ImGui::SameLine();
	ImGui::RadioButton("Z", &dirLight_dir_radioButtons_opt, 2);
	switch (dirLight_dir_radioButtons_opt) {
	case 0:
		ImGui::SliderFloat("", &instance->light->properties.direction.x, -10.0f, 10.0f, "%.2f");
		break;
	case 1:
		ImGui::SliderFloat("", &instance->light->properties.direction.y, -10.0f, 10.0f, "%.2f");
		break;
	case 2:
		ImGui::SliderFloat("", &instance->light->properties.direction.z, -10.0f, 10.0f, "%.2f");
		break;
	}

	if (ImGui::CollapsingHeader("Light Color")) {
		// Ambient color
		if (ImGui::CollapsingHeader("Ambient")) {
			ImGui::ColorPicker3("Ka", &instance->light->properties.color.ambient.x);
		}

		// Diffuse color
		if (ImGui::CollapsingHeader("Diffuse")) {
			ImGui::ColorPicker3("Kd", &instance->light->properties.color.diffuse.x);
		}

		// Specular color
		if (ImGui::CollapsingHeader("Specular")) {
			ImGui::ColorPicker3("Ks", &instance->light->properties.color.specular.x);
		}
	}
}

void UI::pointLightProperties() {

	// Scene selector
	ImGui::Text("Light selector");
	if (ImGui::BeginCombo("##combo_light_selector", current_item_light_selector)) {
		for (int n = 0; n < instance->scene.size(); n++)
		{
			//bool is_selected = (current_item_light_selector == instance->scene[n]->name.c_str()); // You can store your selection however you want, outside or inside your objects
/*			if (ImGui::Selectable(instance->scene[n]->name.c_str(), is_selected)) {
				current_item_light_selector = instance->scene[n]->name.c_str();
				instance->currentScene = n;
				instance->setPickedIndex(-1);
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();*/   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
		}
		ImGui::EndCombo();
	}
	ImGui::Separator();
}

void UI::pickedModelWindow() {
	// If there's an element picked show its info
	if (instance->getPickedIndex() != -1) {
		glm::mat4 viewMatrix = instance->camera.getWorldToViewMatrix();
		float** matrix = new float*[4];
		bool flag = false;
		for (int i = 0; i < 4; i++)
			matrix[i] = new float[4];
		string frameTitle = "Model: " + instance->scene[instance->currentScene]->models[instance->getPickedIndex()]->name;
		ImGui::Begin(frameTitle.c_str(), (bool*)true);
		
		// Set euler angles from quaternion
		if (ImGui::SliderFloat3("Rotation", &instance->scene[instance->currentScene]->models[instance->getPickedIndex()]->rotationAngles.x, 0.0f, 360.0f, "%.2f")) {
			flag = true;
			glm::quat q(glm::radians(instance->scene[instance->currentScene]->models[instance->getPickedIndex()]->rotationAngles));
			instance->scene[instance->currentScene]->models[instance->getPickedIndex()]->rotationMatrix = glm::toMat4(q);
		}
		
		// Set translation matrix
		if (ImGui::DragFloat3("Translation", &instance->scene[instance->currentScene]->models[instance->getPickedIndex()]->position.x,0.5f)) {
			flag = true;
			instance->scene[instance->currentScene]->models[instance->getPickedIndex()]->setTranslationMatrix();
		}

		if (ImGui::DragFloat3("Scaling", &instance->scene[instance->currentScene]->models[instance->getPickedIndex()]->scale.x,0.2f,1.0f,100.0f)) {
			flag = true;
			instance->scene[instance->currentScene]->models[instance->getPickedIndex()]->setScaleMatrix();
		}

		// Update model Matrix if any change has been made
		if (flag)
			instance->scene[instance->currentScene]->models[instance->getPickedIndex()]->computeModelMatrix();

		if (ImGui::IsKeyPressed(GLFW_KEY_T)) {
			cout << "hola" << endl;
		}
		ImGui::End();
	}
}

void UI::directionalLightWindow() {
	ImGui::Begin("Directional light properties");

	ImGui::End();

}

void UI::showMenuFile() {
	//ImGui::MenuItem("(dummy menu)", NULL, false, false);
	if (ImGui::MenuItem("New Scene")) {
		activateModal = "Create Scene##modal";
	}
	if (ImGui::MenuItem("Open Scene")) {
	}

	if (ImGui::MenuItem("Save Scene",NULL,false, instance->currentScene != -1)) {
	}

}

void UI::drawModals() {
	if (activateModal != "")
		ImGui::OpenPopup(activateModal.c_str());

	// Save new model  - modal
	ImGui::SetNextWindowSize(ImVec2(300, 130));
	if (ImGui::BeginPopupModal("Add a new model"))
	{
		ImGui::Text("Please write model's path");
		static char buffer[1024] = "assets/models/catscaled.obj";
		ImGui::InputText("", buffer, IM_ARRAYSIZE(buffer));
		if (ImGui::Button("Add", ImVec2(80, 30))){
			instance->addModel(string(buffer));
			ImGui::CloseCurrentPopup();
			activateModal = "";
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(80, 30)))
		{
			ImGui::CloseCurrentPopup();
			activateModal = "";
		}
		ImGui::EndPopup();
	}

	// Create new scene  - modal
	ImGui::SetNextWindowSize(ImVec2(300, 130));
	if (ImGui::BeginPopupModal("Create Scene##modal",0))
	{
		ImGui::Text("Please name your scene");
		string defaultSceneName = "Scene " + to_string(instance->scene.size());
		char buffer[1024];
		strcpy_s(buffer, defaultSceneName.c_str());

		ImGui::InputText("", buffer, IM_ARRAYSIZE(buffer));
		if (ImGui::Button("Create", ImVec2(80, 30))) {
			instance->createScene(string(buffer));
			ImGui::CloseCurrentPopup();
			activateModal = "";
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(80, 30)))
		{
			ImGui::CloseCurrentPopup();
			activateModal = "";
		}
		ImGui::EndPopup();
	}

	// Add a new light - modal
	ImGui::SetNextWindowSize(ImVec2(300, 130));
	if (ImGui::BeginPopupModal("Add a new light"))
	{
		ImGui::Text("Please name your light");
		string defaultLightName = "Light " /*+ to_string(instance->scene[instance->getPickedIndex()])*/;
		char buffer[1024];
		strcpy_s(buffer, defaultLightName.c_str());
		ImGui::InputText("", buffer, IM_ARRAYSIZE(buffer));
		if (ImGui::Button("Add", ImVec2(80, 30))) {
			instance->addLight(string(buffer));
			ImGui::CloseCurrentPopup();
			activateModal = "";
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(80, 30)))
		{
			ImGui::CloseCurrentPopup();
			activateModal = "";
		}
		ImGui::EndPopup();
	}

}

void UI::showMainMenuBar() {

	if (ImGui::BeginMainMenuBar())
	{
		// File Option
		if (ImGui::BeginMenu("File"))
		{
			showMenuFile();
			ImGui::EndMenu();
		}
		// Scene Option
		if (instance->currentScene != -1) {
			if (ImGui::BeginMenu("Scene")){
				if (ImGui::MenuItem("Add model")) {
					activateModal = "Add a new model";
				}
				if (ImGui::MenuItem("Add light")) {
					activateModal = "Add a new light";
				}
				ImGui::EndMenu();
			}
		}
		// Settings button
		if (settingFlag) {
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(247 / 255.0f, 202 / 255.0f, 22 / 255.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(247 / 255.0f, 202 / 255.0f, 22 / 255.0f, 1.0f));
			if (ImGui::Button("Settings")) {
				settingFlag = settingFlag == true ? false : true;
			}
			ImGui::PopStyleColor(2);
		}
		else {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(3 / 255.0f, 60 / 255.0f, 84 / 255.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(247 / 255.0f, 202 / 255.0f, 22 / 255.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(247 / 255.0f, 202 / 255.0f, 22 / 255.0f, 1.0f));
			if (ImGui::Button("Settings")) {
				settingFlag = settingFlag == true ? false : true;
			}
			ImGui::PopStyleColor(3);
		}

		ImGui::EndMainMenuBar();
	}
}

void UI::draw() {
	// Start new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::BeginFrame();
	ImGuizmo::Enable(true);
	ImGui::GetIO().WantCaptureMouse = true;
	ImGuizmo::DrawGrid(&instance->camera.viewMatrix[0][0], &instance->camera.getPerspectiveMatrix()[0][0], &glm::mat4(1.0f)[0][0], 10.0f);
	ImGuizmo::SetRect(0, 0, instance->windowWidth, instance->windowHeight);
	glm::mat4 matrix;
	//ImGuizmo::Manipulate(&instance->camera.getWorldToViewMatrix()[0][0], &instance->camera.getPerspectiveMatrix()[0][0], ImGuizmo::ROTATE, ImGuizmo::WORLD, &matrix[0][0], NULL, NULL);
	//ImGuizmo::DecomposeMatrixToComponents(&matrix[0][0],&instance->camera.position[0], &instance->camera.viewDirection[0], &glm::vec3(1.0f)[0]);
	ImGuizmo::ViewManipulate(&instance->camera.viewMatrix[0][0],8.0f,ImVec2(instance->windowWidth-128,0),ImVec2(128,128),0x10101010);
	showMainMenuBar();


	// Main configuration window
	settingsWindow();

	// Directional light information
	//directionalLightWindow();
	
	// Picked model window
	pickedModelWindow();

	drawModals(); 

	// Render dear imgui into screen
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::terminate() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
