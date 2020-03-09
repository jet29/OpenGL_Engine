#include "UserInterface.h"
#include "../icarus3D.h"
//#include <ImGuizmo.h>


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
	if (!ImGui_ImplGlfw_InitForOpenGL(window, true) || !ImGui_ImplOpenGL3_Init("#version 330 core"))
		return false;
	ImGui::StyleColorsDark();

	//Get singleton instance;
	instance = icarus3D::Instance();
	return true;
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
				instance->setPickedIndex(-1);
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

	//if (instance->currentScene != -1) {
	//	if (ImGui::CollapsingHeader("Add model", ImGuiTreeNodeFlags_DefaultOpen)) {
	//		static char buffer[1024] = "assets/models/catscaled.obj";
	//		ImGui::InputText("Path", buffer, IM_ARRAYSIZE(buffer));
	//		if (ImGui::Button("Add", ImVec2(0, 0))) {
	//			instance->addModel(string(buffer));
	//		}
	//	}
	//}

	ImGui::Separator();

	if (ImGui::Button("Load Scene", ImVec2(0, 0))) {
		instance->loadScene("scene.json");
	}

	if (ImGui::Button("Save Scene", ImVec2(0, 0))) {
		instance->saveScene();
	}

	if (ImGui::Button("Add model"))
	{
		//flag = true;
	}


	ImGui::End();
}

void UI::pickedModelWindow() {
	// If there's an element picked show its info
	if (instance->getPickedIndex() != -1) {
		glm::mat4 viewMatrix = instance->camera.getWorldToViewMatrix();
		float** matrix = new float*[4];
		for (int i = 0; i < 4; i++)
			matrix[i] = new float[4];
		string frameTitle = "Model: " + instance->scene[instance->currentScene]->models[instance->getPickedIndex()]->name;
		ImGui::Begin(frameTitle.c_str(), (bool*)true);
		
		// Set euler angles from quaternion
		ImGui::SliderFloat3("Rotation", &instance->scene[instance->currentScene]->models[instance->getPickedIndex()]->rotationAngles.x, 0.0f, 360.0f, "%.2f");
		glm::quat q(glm::radians(instance->scene[instance->currentScene]->models[instance->getPickedIndex()]->rotationAngles));
		instance->scene[instance->currentScene]->models[instance->getPickedIndex()]->rotationMatrix = glm::toMat4(q);
		
		// Set translation matrix
		ImGui::DragFloat3("Translation", &instance->scene[instance->currentScene]->models[instance->getPickedIndex()]->position.x);

		if (ImGui::IsKeyPressed(GLFW_KEY_T)) {
			cout << "hola" << endl;
		}
		ImGui::End();
	}
}

void UI::directionalLightWindow() {
	ImGui::Begin("Directional light properties");
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

	if(ImGui::CollapsingHeader("Light Color")) {
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
	ImGui::End();

}

void UI::showMenuFile() {
	//ImGui::MenuItem("(dummy menu)", NULL, false, false);
	if (ImGui::MenuItem("New Scene")) {
		instance->createScene();
		items.push_back("Scene " + to_string((items.size() + 1)));
	}
	if (ImGui::MenuItem("Open", "Ctrl+O")) {}
	if (ImGui::BeginMenu("Open Recent"))
	{
		ImGui::MenuItem("fish_hat.c");
		ImGui::MenuItem("fish_hat.inl");
		ImGui::MenuItem("fish_hat.h");
		if (ImGui::BeginMenu("More.."))
		{
			ImGui::MenuItem("Hello");
			ImGui::MenuItem("Sailor");
			if (ImGui::BeginMenu("Recurse.."))
			{
				//ShowExampleMenuFile();
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}
}

void UI::drawModals() {
	if (flag)
		ImGui::OpenPopup("Add a new model");

	ImGui::SetNextWindowSize(ImVec2(300, 130));
	if (ImGui::BeginPopupModal("Add a new model"))
	{
		ImGui::Text("Please write model's path");
		static char buffer[1024] = "assets/models/catscaled.obj";
		ImGui::InputText("", buffer, IM_ARRAYSIZE(buffer));
		if (ImGui::Button("Add", ImVec2(80, 30))){
			instance->addModel(string(buffer));
			ImGui::CloseCurrentPopup();
			flag = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(80, 30)))
		{
			ImGui::CloseCurrentPopup();
			flag = false;
		}
		ImGui::EndPopup();
	}
}

void UI::showMainMenuBar() {

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			showMenuFile();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "CTRL+X")) {}
			if (ImGui::MenuItem("Copy", "CTRL+C")) {}
			if (ImGui::MenuItem("Paste", "CTRL+V")) {}
			ImGui::EndMenu();
		}
		if (instance->currentScene != -1) {
			if (ImGui::BeginMenu("Scene")){
				if (ImGui::MenuItem("Add model")) {
					flag = true;
				}
				ImGui::EndMenu();
			}
		}
		ImGui::Button("test");
		ImGui::EndMainMenuBar();
	}
}

void UI::draw() {
	// Start new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	//ImGuizmo::BeginFrame();
	ImGui::GetIO().WantCaptureMouse = true;

	showMainMenuBar();
	//showModal();

	//ImGui::Begin("test");
	//ImGuizmo::Enable(true);
	//static const float identityMatrix[16] =
	//{ 1.f, 0.f, 0.f, 0.f,
	//	0.f, 1.f, 0.f, 0.f,
	//	0.f, 0.f, 1.f, 0.f,
	//	0.f, 0.f, 0.f, 1.f };

	//float cameraProjection[16];

	//ImGuizmo::DrawGrid(value_ptr(instance->camera.getWorldToViewMatrix()), cameraProjection, identityMatrix, 10.f);

	//ImGui::End();

	// Main configuration window
	//mainConfigWindow();

	// Directional light information
	//directionalLightWindow();
	
	// Picked model window
	pickedModelWindow();

	drawModals(); 

	// Render dear imgui into screen
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::terminate() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
